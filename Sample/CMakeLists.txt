cmake_minimum_required (VERSION 2.8)
project (Destructo)

file(GLOB Destructo_SRC
	"source/*.h"
	"source/*.cpp"
)

file(GLOB libnoise_src
    "includes/libnoise/noise/*.cpp"
    "includes/libnoise/noise/model/*.cpp"
    "includes/libnoise/noise/module/*.cpp"
)

FILE(GLOB AppleCompatFiles
	"source/apple-compat/SaveDirectory.mm"
	"source/apple-compat/NSFileManager+ApplicationSupport.h"
	"source/apple-compat/NSFileManager+ApplicationSupport.m"
)
source_group(AppleCompat FILES ${AppleCompatFiles})

#Add universal includes
include_directories("includes/json")

# Are we compiling on Window?
IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

	# Destructo Executable
	add_executable(Destructo ${Destructo_SRC})

    # Windows should consume headers bundles with source
    include_directories("includes/SDL")
    include_directories("includes/glew")
    include_directories("includes/glm-0.9.4.4")
    include_directories("includes/libnoise")
    include_directories("includes/curl/windows")

    # Windows Libraries
    target_link_libraries(Destructo "../libs/windows/glew/glew32")
    target_link_libraries(Destructo "../libs/windows/SDL/SDL2")
    target_link_libraries(Destructo "../libs/windows/SDL2_ttf-2.0.12/SDL2_ttf")
    target_link_libraries(Destructo "../libs/windows/SDL2_mixer-2.0.0/SDL2_mixer")
    target_link_libraries(Destructo "../libs/windows/libnoise/libnoise")
    target_link_libraries(Destructo "opengl32.lib")

    # Windows cURL things.  Now does 2013 and 2012
	if (${CMAKE_CXX_COMPILER} MATCHES "11")	
		target_link_libraries(Destructo optimized "../libs/windows/curl/11/libcurl_imp")
		target_link_libraries(Destructo debug "../libs/windows/curl/11/libcurld_imp")
		file(COPY ${PROJECT_SOURCE_DIR}/libs/windows/curl/11/dlls/ DESTINATION ${PROJECT_SOURCE_DIR}/libs/windows/_alldlls/)
	ELSEIF (${CMAKE_CXX_COMPILER} MATCHES "12")
		target_link_libraries(Destructo optimized "../libs/windows/curl/12/libcurl_imp")
		target_link_libraries(Destructo debug "../libs/windows/curl/12/libcurld_imp")
		file(COPY ${PROJECT_SOURCE_DIR}/libs/windows/curl/12/dlls/ DESTINATION ${PROJECT_SOURCE_DIR}/libs/windows/_alldlls/)
	ELSE ()
		message("Warning: unsupported MSVC, curl not linked.")
	ENDIF ()	

	#enable Multiprocessor build
	SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /MP" )
	
	#copy game and dlls
	file(COPY ${PROJECT_SOURCE_DIR}/game/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
	file(COPY ${PROJECT_SOURCE_DIR}/libs/windows/_alldlls/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
	file(COPY ${PROJECT_SOURCE_DIR}/libs/windows/_alldlls/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)

	
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

# Are we compiling on Mac OS X?
IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # Make sure Xcode uses Clang set for libc++ and c++ version 2011
	set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

	# Needed for performing Makefile build on OS X, as above parameters are Xcode specific
	set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -stdlib=libc++ -g -Wall")

	# Setup the names for things
    set( EXE_NAME "Destruct-o")
    set( MACOSX_BUNDLE_INFO_STRING "@EXE_NAME@ - Version 0.0.1" )
    set( MACOSX_BUNDLE_BUNDLE_VERSION 0.0.1 )
    #set( MACOSX_BUNDLE_ICON_FILE "destructo.icns" )
    set( MACOSX_BUNDLE_GUI_IDENTIFIER "com.hoodoonet.destruct-o" )
    set( MACOSX_BUNDLE_BUNDLE_NAME "@EXE_NAME@")

    # Find libraries
	find_library(OPENGL_FRAMEWORK     NAMES OpenGL     PATHS ${PROJECT_SOURCE_DIR}/libs/osx/frameworks)
	find_library(SDL2_FRAMEWORK       NAMES SDL2       PATHS ${PROJECT_SOURCE_DIR}/libs/osx/frameworks)
	find_library(SDL2_TTF_FRAMEWORK   NAMES SDL2_ttf   PATHS ${PROJECT_SOURCE_DIR}/libs/osx/frameworks)
	find_library(SDL2_MIXER_FRAMEWORK NAMES SDL2_mixer PATHS ${PROJECT_SOURCE_DIR}/libs/osx/frameworks)
	find_library(CURL_FRAMEWORK       NAMES curl       PATHS ${PROJECT_SOURCE_DIR}/libs/osx/frameworks)
	find_library(NOISE_FRAMEWORK      NAMES noise      PATHS ${PROJECT_SOURCE_DIR}/libs/osx/frameworks)

	# Copy the resources into the app bundle
	file(GLOB MACOSX_RESOURCES ${PROJECT_SOURCE_DIR}/game/* )
	set_source_files_properties(${MACOSX_RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

	# Copy the frameworks into the app bundle
	file(GLOB MACOSX_FRAMEWORKS ${PROJECT_SOURCE_DIR}/libs/osx/frameworks/* )
	set(MACOSX_FRAMEWORKS ${SDL2_FRAMEWORK} ${SDL2_TTF_FRAMEWORK} ${SDL2_MIXER_FRAMEWORK} ${CURL_FRAMEWORK} ${NOISE_FRAMEWORK})
	set_source_files_properties(${MACOSX_FRAMEWORKS} PROPERTIES MACOSX_PACKAGE_LOCATION Frameworks)

	# Declare the destructo executable
	add_executable(Destructo MACOSX_BUNDLE ${Destructo_SRC} ${AppleCompatFiles} ${MACOSX_RESOURCES} ${MACOSX_FRAMEWORKS})

	# Set the runtime search path to look for frameworks and libraries in the Frameworks folder of the .app
	set_target_properties(Destructo PROPERTIES XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@loader_path @loader_path/../Frameworks")
    
    # Don't strip symbols on copy (for frameworks)
    set_target_properties(Destructo PROPERTIES XCODE_ATTRIBUTE_COPY_PHASE_STRIP "No")

    # Link the frameworks to destructo
    target_link_libraries(Destructo ${SDL2_FRAMEWORK})
    target_link_libraries(Destructo ${SDL2_TTF_FRAMEWORK})
    target_link_libraries(Destructo ${SDL2_MIXER_FRAMEWORK})
    target_link_libraries(Destructo ${OPENGL_FRAMEWORK})
    target_link_libraries(Destructo ${CURL_FRAMEWORK})
    target_link_libraries(Destructo ${NOISE_FRAMEWORK})

    # Pull in the bundled GLEW library
    include_directories("includes/glew")
    get_filename_component(CGLEW "libs/osx/glew/" ABSOLUTE)
    target_link_libraries(Destructo ${CGLEW}/libGLEW.a)

    # Pull in the OpenGL Mathematics Library
    include_directories("includes/glm-0.9.4.4")

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
    #
    # A graphics card and driver supporting at least OpenGL 2.0, recommended 3.3+
    #
    # Graphics cards known to work
    #   - Nvidia GeForce GTX 770    (tested on Mac OS X and Linux)
    #   - Nvidia GeForce GTX 550 Ti (tested on Windows)
    #   - Intel HD Graphics 3000    (tested on Mac OS X)

	# Destructo Executable
	add_executable(Destructo ${Destructo_SRC})

    # Discover include path and library for the SDL2 library
    find_path(SDL_INCLUDE_DIR NAMES SDL2/SDL.h)
    include_directories(${SDL_INCLUDE_DIR})
    find_library(SDL_LIBRARY NAMES SDL2)
    target_link_libraries(Destructo ${SDL_LIBRARY})

    # Discover include path and library for the SDL2_ttf library
    find_path(SDL_TTF_INCLUDE_DIR NAMES SDL2/SDL_ttf.h)
    include_directories(${SDL_TTF_INCLUDE_DIR})
    find_library(SDL_TTF_LIBRARY NAMES SDL2_ttf)
    target_link_libraries(Destructo ${SDL_TTF_LIBRARY})

    # Discover include path and library for the SDL2_mixer library
    find_path(SDL_MIXER_INCLUDE_DIR NAMES SDL2/SDL_mixer.h)
    include_directories(${SDL_MIXER_INCLUDE_DIR})
    find_library(SDL_MIXER_LIBRARY NAMES SDL2_mixer)
    target_link_libraries(Destructo ${SDL_MIXER_LIBRARY})

    # Discover the include path and library for the system OpenGL Library
    find_path(OPENGL_INCLUDE_DIR NAMES GL/gl.h)
    include_directories(${OPENGL_INCLUDE_DIR})
    find_library(OPENGL_LIBRARY NAMES GL)
    target_link_libraries(Destructo ${OPENGL_LIBRARY})

    # Discover the include path and library for the OpenGL Extension Wrangler
    find_path(GLEW_INCLUDE_DIR NAMES GL/glew.h)
    include_directories(${GLEW_INCLUDE_DIR})
    find_library(GLEW_LIBRARY NAMES GLEW)
    target_link_libraries(Destructo ${GLEW_LIBRARY})

    # Discover the include path for the OpenGL Mathematics Library
    find_path(GLM_INCLUDE_DIR NAMES glm/glm.hpp)
    include_directories(${GLM_INCLUDE_DIR})

    # Discover the include path for lib cURL
    find_path(CURL_INCLUDE_DIR NAMES curl/curl.h)
    include_directories(${CURL_INCLUDE_DIR})
    find_library(CURL_LIBRARY NAMES curl)
    target_link_libraries(Destructo ${CURL_LIBRARY})

    # Well shoot.  Was hoping to avoid this
    # Pull in the noise generation library
    include_directories("includes/libnoise")

    # Need to build libnoise for Linux
    add_library(noise STATIC ${libnoise_src})
    target_link_libraries(Destructo noise)

    # Make a define, only used with Linux.  Also, make sure we use c++11
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__LINUX__")
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -Wall")

ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

#Code should be organized
#opengl files are really easy to organize
FILE(GLOB GLFiles
	"source/GL*"
	"source/IOSGL.h"
	"source/AndroidGLEW.h"
	"source/AndroidGLEW.cpp"
	"source/MaxDistanceFog.h"
	"source/MaxDistanceFog.cpp"
	"source/ShaderGroup.h"
	"source/ShaderGroup.cpp"
	"source/TextureCache.h"
	"source/TextureCache.cpp"
	"source/TexturedRect.h"
	"source/TexturedRect.cpp"
)
source_group(OpenGL FILES ${GLFiles})

#actors
FILE(GLOB ActorFiles
	"source/Actor*"
	
)
source_group(Actors FILES ${ActorFiles})

FILE(GLOB AIFiles
	source/AidsAction*
	source/AIDataCache.*
	source/WeaponData.*
	source/LaserWeaponData.*
	source/AIWeaponCache.*
	source/EntityFireWeaponData.*
)
source_group(AI FILES ${AIFiles})

#audio
FILE(GLOB AudioFiles
	"source/Audio*"
	"source/Music.h"
	"source/Music.cpp"
	"source/Effect.h"
	"source/Effect.cpp"
)
source_group(Audio FILES ${AudioFiles})

# Skinned Model Rendering
FILE(GLOB ModelFiles
	"source/Model.*"
	"source/ModelInstance.*"
	"source/ModelGroup.*"
	"source/Animation*"
	"source/Node.*"
	"source/Transform.*"
	"source/Mesh.*"
	"source/VertexAttributes.*"
	"source/Material.*"
	"source/MaterialProgram.*"
)
source_group(Models FILES ${ModelFiles})

# cURL
FILE(GLOB CurlCppFiles
	"source/Curl*"
)
source_group(curlcpp FILES ${CurlCppFiles})

#Dialog/Interface files
FILE(GLOB InterfaceFiles
    source/Button.h
	source/Button.cpp
	source/ComboBox.h
	source/ComboBox.cpp
	source/Control.h
	source/Control.cpp
	source/ControlBase.h
	source/ControlBase.cpp
	source/Font.h
	source/Font.cpp
	source/InputEvent.h
	source/InputEvent.cpp
	source/Label.h
	source/Label.cpp
	source/Listbox.h
	source/Listbox.cpp
	source/NinePatchBinary.h
	source/NinePatchBinary.cpp
	source/Textbox.h
	source/Textbox.cpp
	source/Window.h
	source/Window.cpp
	source/InterfaceGlobals.h
	source/InterfaceGlobals.cpp
	source/RasterizedText.h
	source/RasterizedText.cpp
	source/Shape.h
	source/Shape.cpp
	source/Slider.h
	source/PictureBox.*
	source/PictureButton.*
)
source_group(InterfaceLibrary FILES ${InterfaceFiles})

#Dialog Implementations
FILE(GLOB GameInterfaceFiles
	source/DebugWindow.h
	source/DebugWindow.cpp
	source/ErrorWindow.h
	source/ErrorWindow.cpp
	source/PauseWindow.h
	source/PauseWindow.cpp
	source/PauseWindowMobile.h
	source/PauseWindowMobile.cpp
	source/Notification.h
	source/Notification.cpp
	source/HUD.*
)
source_group(UserInterface FILES ${GameInterfaceFiles})


FILE(GLOB PhysicsFiles
	source/Demo.h
	source/Demo.cpp
	source/DemoWindow.h
	source/DemoWindow.cpp
	source/Interop.h
	source/InteropChain.h
	source/InteropDecoders.h
	source/LinearInterop.h
	source/Particle.h
	source/Particle.cpp
	source/ParticleCloud.h
	source/ParticleCloud.cpp
	source/ParticleData.h
	source/ParticleData.cpp
	source/ParticleSystem.h
	source/ParticleSystem.cpp
	source/ParticleRenderer.h
	source/ParticleRenderer.cpp
	source/PhysicsVoxel.h
	source/PhysicsVoxel.cpp
	source/PhysicsTriangle.h
	source/PhysicsTriangle.cpp
	source/PhysicsSystem.h
	source/PhysicsSystem.cpp
	source/PhysicsProxy.h
	source/PhysicsProxy.cpp
	source/PhysicsActor.h
	source/PhysicsActor.cpp
	source/PhysicsUtilities.h
	source/PhysicsUtilities.cpp
	source/Tracer.h
	source/Tracer.cpp
)
source_group(Physics FILES ${PhysicsFiles})

FILE(GLOB LibraryFiles
	source/lodepng.h
	source/lodepng.cpp
	source/jsoncpp.cpp
	source/noiseutils.h
	source/noiseutils.cpp
	source/base64.h
	source/base64.cpp
)
source_group(Libraries FILES ${LibraryFiles})

FILE(GLOB GameplayFiles
	source/Achievements.h
	source/Achievements.cpp
	source/Achievement.h
	source/Achievement.cpp
	source/AchievementProgress.*
	source/GameData.*
	source/Options.*
	source/GameFactions.*
)
source_group(Gameplay FILES ${GameplayFiles})

FILE(GLOB GameDataFiles
	source/DeviceData.*
	source/RootSavable.*
	source/AccountData.*
)
source_group(GameData FILES ${GameDataFiles})




FILE(GLOB SaveFiles
	source/ReflectionData.h
	source/ReflectionData.cpp
	source/LoadData.h
	source/LoadData.cpp
	source/ReflectionStore.h
	source/ReflectionStore.cpp
	source/Savable.h
	source/Savable.cpp
)
source_group(SaveSystem FILES ${SaveFiles})

FILE(GLOB UtilityFiles
	source/AuxLogger.h
	source/AuxLogger.cpp
	source/GameEvent.h
	source/GameEvent.cpp
	source/OS.h
	source/OS.cpp
    source/SaveDirectory.cpp
	source/Semaphore.h
	source/Semaphore.cpp
	source/FPSCounter.h
	source/FPSCounter.cpp
	source/Rect.h
	source/Rect.cpp
	source/GameEventInterface.h
	source/GameEventSubscriber.h
	source/GameEventSubscriber.cpp
	source/Utilities.h
	source/Utilities.cpp
	source/GameSave.h
	source/GameSave.cpp
	source/LoadingScreen.h
	source/LoadingScreen.cpp
	source/IntRect.h
	source/IntRect.cpp
	source/Network.cpp
	source/Network.h
	source/Autocacher.h
	source/AsyncTask.*
	source/SyncTask.*
	source/MovingAverage.*
	source/SimplePolygon.*
	source/TimeStep*
	source/IntegerGlmTypes.*
)
source_group(Utilities FILES ${UtilityFiles})

FILE(GLOB VoxelFiles
	source/BasicVoxelRenderSystem.h
	source/BasicVoxelRenderSystem.cpp
	source/ContiguousList.h
	source/InstancedVoxelRenderSystem.h
	source/InstancedVoxelRenderSystem.cpp
	source/VoxelDrawSystem.h
	source/VoxelDrawSystem.cpp
	source/VoxelSystem.h
	source/VoxelSystem.cpp
	source/TileCell.h
	source/TileHandler.h
	source/TileHandler.cpp
	source/GameTile.h
	source/GameTile.cpp
	source/TerrainChunk.h
	source/TerrainChunk.cpp
	source/TerrainChunkRenderer.h
	source/TerrainChunkRenderer.cpp
	source/Building.h
	source/Building.cpp
	source/Structure.h
	source/Structure.cpp
	source/LoadingScreen.h
	source/LoadingScreen.cpp
)
source_group(Voxels FILES ${VoxelFiles})

FILE(GLOB CoreFiles
	source/VoxEngine*
	source/GameSystem.h
	source/GameSystem.cpp
	source/Frames.h
	source/Frames.cpp
	source/BaseFrame.h
	source/BaseFrame.cpp
	source/MenuFrame.h
	source/MenuFrame.cpp
	source/PauseFrame.*
	source/stdafx.h
	source/stdafx.cpp
	source/Universal.h
	source/Universal.cpp
	source/LoadInProgess.h
	source/LoadInProgess.cpp
)
source_group(Core FILES ${CoreFiles})

FILE(GLOB GenFiles
	source/TerrainGen.*
	source/CityGen.*
	source/SavableCityData.*
)
source_group(Procedural FILES ${GenFiles})

FILE(GLOB EffectFiles
	source/EffectLaser.h
	source/EffectLaser.cpp
)
source_group(Effects FILES ${EffectFiles})

FILE(GLOB WeaponFiles
	source/Weapon.*
	source/WeaponPulseLaser.*
	source/WeaponLaserCannon.*
	source/WeaponAI*
	source/WeaponModifiers.*
)
source_group(Weapons FILES ${WeaponFiles})

FILE(GLOB ControlFiles
	source/FirstPersonMode.h
	source/FirstPersonMode.cpp
	source/FirstPersonModeMobile.h
	source/FirstPersonModeMobile.cpp
	source/GameCamera.h
	source/GameCamera.cpp
)
source_group(GameControl FILES ${ControlFiles})


##########################################################################################
#start the unit test voxel group
project (DestructoUnitTests)

file(GLOB DestructoUnitTests_SRC
	"unittests/*.h"
	"unittests/*.cpp"
	"source/ContiguousList.h"
)


# Unit Test Executable
#add_executable(DestructoUnitTests ${DestructoUnitTests_SRC})
#SET_TARGET_PROPERTIES(DestructoUnitTests PROPERTIES COMPILE_FLAGS -D__UNIT_TESTING__)

#Reset the project to set the main project name (at least on windows)
project (Destructo)
