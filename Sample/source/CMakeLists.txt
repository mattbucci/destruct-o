cmake_minimum_required (VERSION 2.6)
project (VoxEngine)

file(GLOB VoxEngine_SRC
	"*.h"
	"*.cpp"
)

file(GLOB libnoise_src
    "../includes/libnoise/noise/*.cpp"
    "../includes/libnoise/noise/model/*.cpp"
    "../includes/libnoise/noise/module/*.cpp"
)

# VoxEngine Executable
add_executable(VoxEngine ${VoxEngine_SRC})

# Are we compiling on Window?
IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

    # Windows should consume headers bundles with source
    include_directories("../includes/SDL")
    include_directories("../includes/glew")
    include_directories("../includes/glm-0.9.4.4")
    include_directories("../includes/libnoise")

    # Windows Libraries
    target_link_libraries(VoxEngine "../libs/windows/glew/glew32")
    target_link_libraries(VoxEngine "../libs/windows/SDL/SDL2")
    target_link_libraries(VoxEngine "../libs/windows/SDL2_ttf-2.0.12/SDL2_ttf")
    target_link_libraries(VoxEngine "../libs/windows/SDL2_mixer-2.0.0/SDL2_mixer")
    target_link_libraries(VoxEngine "../libs/windows/libnoise/libnoise")
    target_link_libraries(VoxEngine "opengl32.lib")

ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

# Are we compiling on Mac OS X?
IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    # Find the SDL2 Framework
    find_library(SDL2_FRAMEWORK NAMES SDL2)
    target_link_libraries(VoxEngine ${SDL2_FRAMEWORK})

    # Find the SDL2_ttf Framework
    find_library(SDL2_TTF_FRAMEWORK NAMES SDL2_ttf)
    target_link_libraries(VoxEngine ${SDL2_TTF_FRAMEWORK})

    # Find the SDL2_mixer Framework
    find_library(SDL2_MIXER_FRAMEWORK NAMES SDL2_mixer)
    target_link_libraries(VoxEngine ${SDL2_MIXER_FRAMEWORK})
    
    # Find the OpenGL Framework
    find_library(OPENGL_FRAMEWORK NAMES OpenGL)
    target_link_libraries(VoxEngine ${OPENGL_FRAMEWORK})
    
    # Find the cURL Framework
    find_library(CURL_FRAMEWORK NAMES curl)
    target_link_libraries(VoxEngine ${CURL_FRAMEWORK})

    # Fine the noise Framework
    find_library(NOISE_FRAMEWORK NAMES noise)
    target_link_libraries(VoxEngine ${NOISE_FRAMEWORK})

    # Pull in the bundled GLEW library
    include_directories("../includes/glew")
    get_filename_component(CGLEW "../libs/osx/glew/" ABSOLUTE)
    target_link_libraries(VoxEngine ${CGLEW}/libGLEW.a)
    
    # Pull in the OpenGL Mathematics Library
    include_directories("../includes/glm-0.9.4.4")

    # Make sure Xcode uses Clang set for libc++ and c++ version 2011
    set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

    # Needed for performing Makefile build on OS X, as above parameters are Xcode specific
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -stdlib=libc++ -g -Wall")

ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

# Are we compiling on Linux?
IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

    # Linux is very good with package management and users should understand about the
    # highly organized system they run on.  Either way, depend on the libraries
    # being installed on the system.  OpenSuSE 13.1 had all dependecies in its repos

    # Required Libraries
    #
    # OpenGL Runtime (Mesa-libGL1, tested w/ 9.2.2-61.6.1) - http://www.mesa3d.org/
    # OpenGL Extension Wrangler (GLEW) - http://glew.sourceforge.net/
    # OpenGL Mathematics Library (GLM) - http://glm.g-truc.net/
    #
    # Simple Direct Media Layer (requires 2.0.0 or greater) - http://libsdl.org/download-2.0.php
    # Truetype Font Library for SDL (requires 2.0.12 or greater) - http://www.libsdl.org/projects/SDL_ttf/
    # Audio Mixer Library for SDL (requires 2.0.0 or greater)
    #
    # libcurl (version 7.33.0 or greater) - http://curl.haxx.se/libcurl/
    # libnoise (version 1.0.0 or greater) - http://libnoise.sourceforge.net/
    #
    # A graphics card and driver supporting at least OpenGL 2.0, recommended 3.3+
    #
    # Graphics cards known to work
    #   - Nvidia GeForce GTX 770    (tested on Mac OS X and Linux)
    #   - Nvidia GeForce GTX 550 Ti (tested on Windows)
    #   - Intel HD Graphics 3000    (tested on Mac OS X)

    # Discover include path and library for the SDL2 library
    find_path(SDL_INCLUDE_DIR NAMES SDL2/SDL.h)
    include_directories(${SDL_INCLUDE_DIR})
    find_library(SDL_LIBRARY NAMES SDL2)
    target_link_libraries(VoxEngine ${SDL_LIBRARY})

    # Discover include path and library for the SDL2_ttf library
    find_path(SDL_TTF_INCLUDE_DIR NAMES SDL2/SDL_ttf.h)
    include_directories(${SDL_TTF_INCLUDE_DIR})
    find_library(SDL_TTF_LIBRARY NAMES SDL2_ttf)
    target_link_libraries(VoxEngine ${SDL_TTF_LIBRARY})

    # Discover include path and library for the SDL2_mixer library
    find_path(SDL_MIXER_INCLUDE_DIR NAMES SDL2/SDL_mixer.h)
    include_directories(${SDL_MIXER_INCLUDE_DIR})
    find_library(SDL_MIXER_LIBRARY NAMES SDL2_mixer)
    target_link_libraries(VoxEngine ${SDL_MIXER_LIBRARY})

    # Discover the include path and library for the system OpenGL Library
    find_path(OPENGL_INCLUDE_DIR NAMES GL/gl.h)
    include_directories(${OPENGL_INCLUDE_DIR})
    find_library(OPENGL_LIBRARY NAMES GL)
    target_link_libraries(VoxEngine ${OPENGL_LIBRARY})

    # Discover the include path and library for the OpenGL Extension Wrangler
    find_path(GLEW_INCLUDE_DIR NAMES GL/glew.h)
    include_directories(${GLEW_INCLUDE_DIR})
    find_library(GLEW_LIBRARY NAMES GLEW)
    target_link_libraries(VoxEngine ${GLEW_LIBRARY})

    # Discover the include path for the OpenGL Mathematics Library
    find_path(GLM_INCLUDE_DIR NAMES glm/glm.hpp)
    include_directories(${GLM_INCLUDE_DIR})

    # Discover the include path for lib cURL
    find_path(CURL_INCLUDE_DIR NAMES curl/curl.h)
    include_directories(${CURL_INCLUDE_DIR})
    find_library(CURL_LIBRARY NAMES curl)
    target_link_libraries(VoxEngine ${CURL_LIBRARY})
    
    # Well shoot.  Was hoping to avoid this
    # Pull in the noise generation library
    include_directories("../includes/libnoise")
    
    # Need to build libnoise for Linux
    add_library(noise STATIC ${libnoise_src})
    target_link_libraries(VoxEngine noise)

    # Make a define, only used with Linux.  Also, make sure we use c++11
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__LINUX__")
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -Wall")

ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

#An attempt to copy the needed files over on run, doesn't work right now
#add_custom_command(TARGET MyTarget PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/game/Interface $<TARGET_FILE_DIR:MyTarget>)

#Code should be organized later, but currently
#doesn't work properly in visual studio
#FILE(GLOB GLFiles "GL*")
#source_group(OpenGL FILES ${GLFiles})
