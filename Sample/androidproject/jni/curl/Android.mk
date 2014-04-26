LOCAL_PATH := $(call my-dir)

# include libandprof.a in the build
include $(CLEAR_VARS)
LOCAL_MODULE := curl
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libcurl.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
include $(PREBUILT_STATIC_LIBRARY)
