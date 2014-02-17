LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
 
LOCAL_LDLIBS    := -lGLESv1_CM -llog -lGLESv2
 
LOCAL_SHARED_LIBRARIES := SDL2 SDL2_ttf SDL2_mixer
 
LOCAL_MODULE    := main

FILE_LIST := $(wildcard $(LOCAL_PATH)/../../../source/*.cpp)
#Added libnoise to the FILE_LIST
FILE_LIST += $(wildcard $(LOCAL_PATH)/noise/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/noise/model/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/noise/module/*.cpp)

LOCAL_SRC_FILES := SDL_android_main.cpp $(FILE_LIST:$(LOCAL_PATH)/%=%)
 
include $(BUILD_SHARED_LIBRARY)