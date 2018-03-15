#
# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# This file is executed by build/envsetup.sh, and can use anything
# defined in envsetup.sh.
#
# In particular, you can add lunch options with the add_lunch_combo
# function: add_lunch_combo generic-eng

#!/bin/bash

function cdevice()
{	
	cd $DEVICE
}

function cout()
{
	cd $OUT	
}

function extract-bsp()
{
	LICHEE_DIR=$ANDROID_BUILD_TOP/../lichee
    CHIP_VERSION=$(get_build_var SW_CHIP_PLATFORM)
    if [ "$CHIP_VERSION" = "H8" ];then
	    LINUXOUT_DIR=$LICHEE_DIR/out/sun8iw6p1/android/common
	elif [ "$CHIP_VERSION" = "H3" ];then
		LINUXOUT_DIR=$LICHEE_DIR/out/sun8iw7p1/android/common
    elif [ "$CHIP_VERSION" = "A80" ];then
	    LINUXOUT_DIR=$LICHEE_DIR/out/sun9iw1p1/android/common
    else
        echo "unknow CHIP_VERSION $CHIP_VERSION"
        return
    fi
	LINUXOUT_MODULE_DIR=$LINUXOUT_DIR/lib/modules/*/*
	CURDIR=$PWD

	cd $DEVICE

	#extract kernel
	if [ -f kernel ] ; then
		rm kernel
	fi
	cp $LINUXOUT_DIR/bImage kernel
	echo "$DEVICE/bImage copied!"

	#extract linux modules
	if [ -d modules ] ; then
		rm -rf modules
	fi
	mkdir -p modules/modules
	cp -rf $LINUXOUT_MODULE_DIR modules/modules
	echo "$DEVICE/modules copied!"
	chmod 0755 modules/modules/*

# create modules.mk
    (cat << EOF) > ./modules/modules.mk
# modules.mk generate by extract-files.sh, do not edit it.
PRODUCT_COPY_FILES += \\
	\$(call find-copy-subdir-files,*,\$(LOCAL_PATH)/modules,system/vendor/modules)
EOF

	cd $CURDIR
}

function verity_file_init()
{
    echo "verity_file_init"
	cp -rf $DEVICE/verity ${OUT}
	$DEVICE/verity/gen_file_verify_block.sh ${OUT}/system
	cp -f ${OUT}/verity/verity_block ${OUT}/verity_block.img
}

function pack()
{

    pwd=$PWD
    T=$(gettop)
    export ANDROID_IMAGE_OUT=$OUT
    export PACKAGE=$T/../lichee/tools/pack

    chip=$(get_build_var SW_CHIP_PLATFORM)
    if [ "x$chip" = "xH3" ]; then
        chip=sun8iw7p1
    elif [ "x$chip" = "xH8" ]; then
        chip=sun8iw6p1
    elif [ "x$chip" = "xA80" ]; then
        chip=sun9iw1p1
    else
        echo "unknow chip platform : ${chip}"
        return 1
    fi

    board=$(get_build_var PACK_BOARD)
    if [ "x$board" = "x" ]; then
        echo "unknow board"
        return 1
    fi

    platform=android
    debug=uart0
    mode=normal
    function=none
    sigmode=none
    no_img=none

    secure_product=$(get_build_var SECURE_PRODUCT)
    secure_boot=$(get_build_var SECURE_BOOT)

    if [ "x$secure_boot" != "x" ]; then
        sigmode=secure
        function=prev_refurbish
    fi
    if [ "x$secure_product" != "x" ]; then
        function=none
    fi

    local OPTIND
    while getopts :c:p:b:dsmfn arg
    do
        case $arg in
            c)
                chip=$OPTARG
                ;;
            p)
                platform=$OPTARG
                ;;
            b)
                board=$OPTARG
                ;;
            d)
                debug=card0
                ;;
            m)
                mode=ota_test
                ;;
            n)
                no_img="no_img"
                ;;
            s)
                sigmode=secure
                ;;
            f)
                function=prev_refurbish
                ;;
            ?)
                ;;
        esac
    done
    echo "============================================"
    echo "chip       = $chip"
    echo "platform   = $platform"
    echo "board      = $board"
    echo "debug      = $debug"
    echo "mode       = $mode"
    echo "function   = $function"
    echo "sigmode    = $sigmode"
    echo "============================================"

    if [ "x$sigmode" == "xsecure" ]; then
        verity_file_init
    fi
    cd $PACKAGE
    $PACKAGE/pack -c $chip -p $platform -b $board -d $debug -s $sigmode -f ${function} -m $mode -n ${no_img}
    cd $pwd
}

function get_uboot()
{
    pack -s -f -n $@
    if [ ! -e $OUT/bootloader ] ; then
        mkdir $OUT/bootloader
    fi
    rm -rf $OUT/bootloader/*
    cp -v $PACKAGE/out/bootloader.fex $OUT
    cp -r $PACKAGE/out/boot-resource/* $OUT/bootloader
    echo "\"$PACKAGE/out/boot-resource/* -> $OUT/bootloader/\""
    cp -v $PACKAGE/out/env.fex $OUT
    cp -v $PACKAGE/out/boot0_nand.fex $OUT
    cp -v $PACKAGE/out/boot0_sdcard.fex $OUT
    cp -v $PACKAGE/out/u-boot.fex $OUT/uboot_nand.fex
    cp -v $PACKAGE/out/u-boot.fex $OUT/uboot_sdcard.fex

	if [ -e $PACKAGE/out/toc0.fex ]; then
		cp -v $PACKAGE/out/toc0.fex $OUT/toc0.fex
	fi

	if [ -e $PACKAGE/out/toc1.fex ]; then
		cp -v $PACKAGE/out/toc1.fex $OUT/toc1.fex
	fi

	if [ -e $PACKAGE/out/verity_block.fex ]; then
		cp -v $PACKAGE/out/verity_block.fex $OUT/verity_block.fex
	fi
}

function make_ota_target_file()
{
  get_uboot
  echo "rm $OUT/obj/PACKAGING/target_files_intermediates/"
  rm -rf $OUT/obj/PACKAGING/target_files_intermediates/
  echo "---make target-files-package---"
  make target-files-package

}

function make_ota_package()
{
  get_uboot
  echo "rm $OUT/obj/PACKAGING/target_files_intermediates/"
  rm -rf $OUT/obj/PACKAGING/target_files_intermediates/
  echo "----make otapackage ----"
  make otapackage -j8
}

function make_ota_package_inc()
{
  mv *.zip old_target_files.zip
  get_uboot
  echo "rm $OUT/obj/PACKAGING/target_files_intermediates/"
  rm -rf $OUT/obj/PACKAGING/target_files_intermediates/
  echo "----make otapackage_inc----"
  make otapackage_inc
}

function make_ota_boottest_package()
{
  get_uboot -m
  echo "rm $OUT/obj/PACKAGING/target_files_intermediates/"
  rm -rf $OUT/obj/PACKAGING/target_files_intermediates/
  echo "----make otapackage ----"
  make otapackage_boottest -j8
}

function verity_key_init()
{
	$DEVICE/verity/gen_dm_verity_key.sh
	cp -f $DEVICE/verity/rsa_key/verity_key $OUT/root
}

function make_toc_package()
{
	$DEVICE/package_toc.sh
}

