LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
 
LOCAL_LDLIBS    := -lGLESv1_CM -llog -lGLESv2
 
LOCAL_SHARED_LIBRARIES := SDL2 SDL2_ttf
 
LOCAL_MODULE    := main

FILE_LIST := $(wildcard $(LOCAL_PATH)/../../../source/*.cpp)
LOCAL_SRC_FILES := SDL_android_main.cpp $(FILE_LIST:$(LOCAL_PATH)/%=%)
 
include $(BUILD_SHARED_LIBRARY)