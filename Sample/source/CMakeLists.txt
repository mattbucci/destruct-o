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

#Add universal includes
include_directories("../includes/json")

# Are we compiling on Window?
IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/game/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

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
    
    # Find the OpenAssimp Framework
    find_library(ASSIMP_FRAMEWORK NAMES assimp)
    target_link_libraries(VoxEngine ${ASSIMP_FRAMEWORK})

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
    # Audio Mixer Library for SDL (requires 2.0.0 or greater) - http://www.libsdl.org/projects/SDL_mixer/
    #
    # libcurl (version 7.33.0 or greater) - http://curl.haxx.se/libcurl/
    # libnoise (version 1.0.0 or greater) - http://libnoise.sourceforge.net/
    # Open Asset Importer (version 3.0 or greater) - http://assimp.sourceforge.net
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

#Code should be organized
#opengl files are really easy to organize
FILE(GLOB GLFiles "GL*" "IOSGL.h" "AndroidGLEW.h" "AndroidGLEW.cpp" "MaxDistanceFog.h" "MaxDistanceFog.cpp" "ShaderGroup.h" "ShaderGroup.cpp" "TextureCache.h" "TextureCache.cpp")
source_group(OpenGL FILES ${GLFiles})

#actors
FILE(GLOB ActorFiles "Actor*")
source_group(Actors FILES ${ActorFiles})

#audio
FILE(GLOB AudioFiles "Audio*" "Music.h" "Music.cpp" "Effect.h" "Effect.cpp")
source_group(Audio FILES ${AudioFiles})

#Dialog/Interface files
FILE(GLOB InterfaceFiles
    Button.h
	Button.cpp
	ComboBox.h
	ComboBox.cpp
	Control.h
	Control.cpp
	ControlBase.h
	ControlBase.cpp
	DebugWindow.h
	DebugWindow.cpp
	ErrorWindow.h
	ErrorWindow.cpp
	Font.h
	Font.cpp
	InputEvent.h
	InputEvent.cpp
	Label.h
	Label.cpp
	Listbox.h
	Listbox.cpp
	NinePatchBinary.h
	NinePatchBinary.cpp
	Textbox.h
	Textbox.cpp
	Window.h
	Window.cpp
	InterfaceGlobals.h
	InterfaceGlobals.cpp
	RasterizedText.h
	RasterizedText.cpp
	Shape.h
	Shape.cpp
	Notification.h
	Notifiaction.cpp
)
source_group(Interface FILES ${InterfaceFiles})

FILE(GLOB PhysicsFiles
	Demo.h
	Demo.cpp
	DemoWindow.h
	DemoWindow.cpp
	Interop.h
	InteropChain.h
	InteropDecoders.h
	LinearInterop.h
	Particle.h
	Particle.cpp
	ParticleCloud.h
	ParticleCloud.cpp
	ParticleData.h
	ParticleData.cpp
	ParticleSystem.h
	ParticleSystem.cpp
	ParticleRenderer.h
	ParticleRenderer.cpp
	PhysicsVoxel.h
	PhysicsVoxel.cpp
	PhysicsTriangle.h
	PhysicsTriangle.cpp
	PhysicsSystem.h
	PhysicsSystem.cpp
)
source_group(Physics FILES ${PhysicsFiles})

FILE(GLOB LibraryFiles
	lodepng.h
	lodepng.cpp
	jsoncpp.cpp
	noiseutils.h
	noiseutils.cpp	
)
source_group(Libraries FILES ${LibraryFiles})

FILE(GLOB SaveFiles
	ReflectionData.h
	ReflectionData.cpp
	LoadData.h
	LoadData.cpp
	ReflectionStore.h
	ReflectionStore.cpp
	Savable.h
	Savable.cpp
)
source_group(Saving FILES ${SaveFiles})

FILE(GLOB UtilityFiles
	AuxLogger.h
	AuxLogger.cpp
	GameEvent.h
	GameEvent.cpp
	OS.h
	OS.cpp
	Semaphore.h
	Semaphore.cpp
	FPSCounter.h
	FPSCounter.cpp
	Rect.h
	Rect.cpp
	GameEventInterface.h
	GameEventSubscriber.h
	GameEventSubscriber.cpp
	Utilities.h
	Utilities.cpp
	GameSave.h
	GameSave.cpp
	ViewDistanceCalc.h
	ViewDistanceCalc.cpp
	LoadingScreen.h
	LoadingScreen.cpp
)
source_group(Utilities FILES ${UtilityFiles})

FILE(GLOB VoxelFiles
	BasicVoxelRenderSystem.h
	BasicVoxelRenderSystem.cpp
	ContiguousList.h
	InstancedVoxelRenderSystem.h
	InstancedVoxelRenderSystem.cpp
	VoxelDrawSystem.h
	VoxelDrawSystem.cpp
	VoxelSystem.h
	VoxelSystem.cpp
	TileCell.h
	TileHandler.h
	TileHandler.cpp
	GameTile.h
	GameTile.cpp
)
source_group(Voxels FILES ${VoxelFiles})

FILE(GLOB CoreFiles
	VoxEngine.h
	VoxEngine.cpp
	GameSystem.h
	GameSystem.cpp
	Frames.h
	Frames.cpp
	BaseFrame.h
	BaseFrame.cpp
	stdafx.h
	stdafx.cpp
)
source_group(Core FILES ${CoreFiles})

FILE(GLOB GenFiles
	TerrainGen.h
	TerrainGen.cpp
	CityGen.h
	CityGen.cpp
)
source_group(Procedural FILES ${GenFiles})

FILE(GLOB ControlFiles
	FirstPersonMode.h
	FirstPersonMode.cpp
	FirstPersonModeMobile.h
	FirstPersonModeMobile.cpp
	LoadingScreen.h
	LoadingScreen.cpp
	GameCamera.h
	GameCamera.cpp
)
source_group(GameControl FILES ${ControlFiles})


##########################################################################################
#start the unit test voxel group
project (VoxEngineUnitTests)

file(GLOB VoxEngineUnitTests_SRC
	"../unittests/*.h"
	"../unittests/*.cpp"
	"ContiguousList.h"
)


# Unit Test Executable
add_executable(VoxEngineUnitTests ${VoxEngineUnitTests_SRC})
SET_TARGET_PROPERTIES(VoxEngineUnitTests PROPERTIES COMPILE_FLAGS -D__UNIT_TESTING__)


#Add universal includes
include_directories("../includes/json")

#Reset the project to set the main project name (at least on windows)
project (VoxEngine)
