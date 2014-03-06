
# use this to select gcc instead of clang
NDK_TOOLCHAIN_VERSION := 4.8

# then enable c++11 extentions in source code
APP_CPPFLAGS += -std=c++11
# or use APP_CPPFLAGS := -std=gnu++11

APP_STL:=gnustl_static
#need exceptions for libnoise
APP_CPPFLAGS += -fexceptions