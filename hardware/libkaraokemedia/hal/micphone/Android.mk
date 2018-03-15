LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := micphone.dolphin
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_SRC_FILES := micphone.c
LOCAL_C_INCLUDES := \
	external/tinyalsa/include \
	device/softwinner/common/hardware/libaudioutils/include
LOCAL_SHARED_LIBRARIES := liblog libcutils libtinyalsa 
LOCAL_SHARED_LIBRARIES += libaudutils
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
