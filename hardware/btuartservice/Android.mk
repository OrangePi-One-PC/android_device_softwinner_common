LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := btuartservice.c


LOCAL_MODULE := btuartservice
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := libutils libcutils libc liblog

include $(BUILD_EXECUTABLE)