cmake_minimum_required (VERSION 2.6)
project (VoxEngine)

file(GLOB VoxEngine_SRC
	"*.h"
	"*.cpp"
)

add_executable(VoxEngine ${VoxEngine_SRC})

IF (${WIN32})
	
ENDIF (${WIN32})

include_directories("../includes/glew")

include_directories("../includes/glm-0.9.4.4")
include_directories("../includes/SDL")

IF (${WIN32})
	target_link_libraries(VoxEngine "../libs/windows/glew/glew32")
	target_link_libraries(VoxEngine "../libs/windows/SDL/SDL2")
	target_link_libraries(VoxEngine "../libs/windows/SDL2_ttf-2.0.12/SDL2_ttf")
	target_link_libraries(VoxEngine "opengl32.lib")
ENDIF (${WIN32})
IF (${APPLE})
	find_library(SDL NAMES sdl2)
	find_library(SDL_TTF NAMES sdl2_ttf)
	find_library(OPENGL NAMES OpenGL)
	target_link_libraries(VoxEngine ${SDL})
	target_link_libraries(VoxEngine ${SDL_TTF})
	target_link_libraries(VoxEngine ${OPENGL})
	get_filename_component(CGLEW "../libs/osx/glew/" ABSOLUTE)
	target_link_libraries(VoxEngine ${CGLEW}/libGLEW.a)

	set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

ENDIF (${APPLE})

#An attempt to copy the needed files over on run, doesn't work right now
#add_custom_command(TARGET MyTarget PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/game/Interface $<TARGET_FILE_DIR:MyTarget>)

#Code should be organized later, but currently
#doesn't work properly in visual studio
#FILE(GLOB GLFiles "GL*")
#source_group(OpenGL FILES ${GLFiles})