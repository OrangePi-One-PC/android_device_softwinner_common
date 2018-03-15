$(call inherit-product, $(SRC_TARGET_DIR)/product/tvd_base.mk)

#------------------ tools -------------------------

-include device/softwinner/common/tools/tools.mk

# file system
PRODUCT_PACKAGES += \
	e2fsck \
	libext2fs \
	libext2_blkid \
	libext2_uuid \
	libext2_profile \
	libext2_com_err \
	libext2_e2p \
	make_ext4fs

RECOVERY_PRIVATE_TOOLS += \
	device/softwinner/common/tools/mount.exfat

PRODUCT_COPY_FILES += \
	device/softwinner/common/tools/init_parttion.sh:root/sbin/init_parttion.sh \
    device/softwinner/common/tools/sensors.sh:system/bin/sensors.sh

#-------------------------------------------------

#------------------ usb -------------------------
# usb accessory
PRODUCT_PACKAGES += \
	com.android.future.usb.accessory

PRODUCT_COPY_FILES += \
	frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
	frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml

#-------------------------------------------------

#------------------ drm (widevine) -------------------------
PRODUCT_PROPERTY_OVERRIDES += \
	drm.service.enabled=true
PRODUCT_PACKAGES += \
	com.google.widevine.software.drm.xml \
	com.google.widevine.software.drm \
	libdrmwvmplugin \
	libwvm \
	libWVStreamControlAPI_L3 \
	libwvdrm_L3 \
	libdrmdecrypt \
	libwvdrmengine

#-------------------------------------------------

#---------------------- ir ---------------------------

PRODUCT_PACKAGES += \
	multi_ir

include frameworks/av/media/libcedarc/libcdclist.mk
include frameworks/av/media/libcedarx/libcdxlist.mk

PRODUCT_COPY_FILES += \
    device/softwinner/common/configs/keylayout/sunxi-ir.kl:system/usr/keylayout/sunxi-ir.kl \
    device/softwinner/common/configs/keylayout/customer_ir_9f00.kl:system/usr/keylayout/customer_ir_9f00.kl \
    device/softwinner/common/configs/keylayout/customer_ir_dd22.kl:system/usr/keylayout/customer_ir_dd22.kl \
    device/softwinner/common/configs/keylayout/customer_ir_fb04.kl:system/usr/keylayout/customer_ir_fb04.kl \
    device/softwinner/common/configs/keylayout/customer_ir_ff00.kl:system/usr/keylayout/customer_ir_ff00.kl

# define virtual mouse key
PRODUCT_PROPERTY_OVERRIDES += \
        ro.softmouse.left.code=21 \
        ro.softmouse.right.code=22 \
        ro.softmouse.top.code=19 \
        ro.softmouse.bottom.code=20 \
        ro.softmouse.leftbtn.code=23 \
        ro.softmouse.midbtn.code=-1 \
        ro.softmouse.rightbtn.code=-1
#-------------------------------------------------

#---------------------- pppoe ---------------------------
PRODUCT_PACKAGES += \
    pppoe
PRODUCT_COPY_FILES += \
    external/ppp/pppoe/script/ip-up-pppoe:system/etc/ppp/ip-up-pppoe \
    external/ppp/pppoe/script/ip-down-pppoe:system/etc/ppp/ip-down-pppoe \
    external/ppp/pppoe/script/pppoe-options:system/etc/ppp/peers/pppoe-options \
    external/ppp/pppoe/script/pppoe-connect:system/bin/pppoe-connect \
    external/ppp/pppoe/script/pppoe-disconnect:system/bin/pppoe-disconnect

#-------------------------------------------------

#---------------------- trim ---------------------------

PRODUCT_PACKAGES += \
	nand_trim

#-------------------------------------------------

#------------------ Dragon Series apk --------------------------
PRODUCT_PACKAGES += \
    DragonAging \
    DragonSN \
    DragonBox

PRODUCT_PROPERTY_OVERRIDES += \
	ro.sw.testapkpackage=com.softwinner.dragonbox \
	ro.sw.testapkclass=com.softwinner.dragonbox.Main \
	ro.sw.testapkconfigclass=com.softwinner.dragonbox.Configuration \
	ro.sw.agingtestpackage=com.softwinner.agingdragonbox \
	ro.sw.agingtestclass=com.softwinner.agingdragonbox.Main \
	ro.sw.snwritepackage=com.allwinnertech.dragonsn \
	ro.sw.snwriteclass=com.allwinnertech.dragonsn.DragonSNActivity

#-------------------------------------------------

#Homlet additional api
#isomount && securefile && gpioservice
PRODUCT_PACKAGES += \
    isomountmanagerservice \
    libisomountmanager_jni \
    libisomountmanagerservice \
    systemmixservice \
    gpioservice \
    libgpio_jni \
    libgpioservice \
    libsystemmix_jni \
    libadmanager_jni \
    libsystemmixservice \
    libsecurefile_jni \
    libsecurefileservice \
    securefileserver \
    libconfig_jni \
    libswconfig \
    libjni_swos \
    libkaraokejni \
    libaudutils \
    micphone.dolphin \
	libsst

#audio-lib
PRODUCT_PACKAGES += \
	libaudio-resampler

PRODUCT_PROPERTY_OVERRIDES += \
	wifi.interface=wlan0 \
	wifi.supplicant_scan_interval=15 \
	keyguard.no_require_sim=true \
	ro.sys.network_location=true \
	af.resampler.quality=4

PRODUCT_PROPERTY_OVERRIDES += \
    ro.product.firmware=Homlet4.4.2-Qin2-v1.2release

# setting default audio output/input
# "AUDIO_CODEC","AUDIO_HDMI","AUDIO_SPDIF","AUDIO_I2S", etc.
PRODUCT_PROPERTY_OVERRIDES += \
	audio.output.active=AUDIO_CODEC,AUDIO_HDMI \
	audio.input.active=AUDIO_CODEC


#display property
PRODUCT_PROPERTY_OVERRIDES += \
	ro.sf.showhdmisettings=7 \
	persist.sys.disp_init_exit=0

PRODUCT_PROPERTY_OVERRIDES += \
	ro.kernel.android.checkjni=0

PRODUCT_PROPERTY_OVERRIDES += \
	ro.opengles.version=196608

PRODUCT_PROPERTY_OVERRIDES += \
	persist.sys.strictmode.visual=0 \
	persist.sys.strictmode.disable=1

#add for hwc debug
PRODUCT_PROPERTY_OVERRIDES += \
	debug.hwc.showfps=0

# Enabling type-precise GC results in larger optimized DEX files.  The
# additional storage requirements for ".odex" files can cause /system
# to overflow on some devices, so this is configured separately for
# each product.
PRODUCT_TAGS += dalvik.gc.type-precise

# if DISPLAY_BUILD_NUMBER := true then
# BUILD_DISPLAY_ID := $(BUILD_ID).$(BUILD_NUMBER)
# required by gms.
DISPLAY_BUILD_NUMBER := true
BUILD_NUMBER := $(shell date +%Y%m%d)

