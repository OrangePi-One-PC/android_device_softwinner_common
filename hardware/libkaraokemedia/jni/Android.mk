LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	com_karaoke_jni.cpp	\
	com_karaoke_micphone.cpp 	\
	com_karaoke_rtsoundeffects.cpp
	#com_karaoke_mediaplayerex.cpp \

LOCAL_SHARED_LIBRARIES := \
    libandroid_runtime \
    libnativehelper \
    libutils \
    libcutils \
	libhardware

LOCAL_C_INCLUDES := \
    frameworks/base/core/jni \
    $(JNI_H_INCLUDE) 

LOCAL_MODULE := libkaraokejni

include $(BUILD_SHARED_LIBRARY)
