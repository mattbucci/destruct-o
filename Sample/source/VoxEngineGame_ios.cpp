#include "stdafx.h"
#include "VoxEngine.h"
#include "OS.h"

#ifdef __IPHONEOS__

//Do anything necessary to build the render context
void VoxEngine::BuildRenderContext() {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

	displayWindow = BuildSDLContext(2,0,0.0f);
	GLSLVersion = 110;
	//For iOS, globally force the newer opengl version
	OpenglVersion = 31;

	if (displayWindow == NULL) {
		cout << "Failed to open any opengl context on this device. Your device is too out dated, sorry.\n";
		SDL_Quit();
		return;
	}

    // Setup the animation callback for iOS
    SDL_iPhoneSetAnimationCallback(displayWindow, 1, &iOSAnimationCallback, NULL);

	//Get the current window size
	SDL_GetWindowSize(displayWindow,&curWidth,&curHeight);
}

// Called by the iOS animation callback
void iOSAnimationCallback(void *context)
{
    VoxEngine::iOSRenderRequested = true;
    VoxEngine::iOSRenderTime = OS::Now();
}

//returns NULL if that context couldn't be constructed
SDL_Window* VoxEngine::BuildSDLContext(int openglMajorVersion, int openglMinorVersion, float requiredGLSLVersion) {
	// Request double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	// Request a 32 bit depth buffer
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
	
	// Request 32 bit color buffer (RGBA8888)
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	
	// Request OpenGL 3.2
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, openglMajorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, openglMinorVersion);


	SDL_Window* displayWindow;
	// On mobile, create a fullscreen display the size of the device's screen
	SDL_DisplayMode displayMode;
	SDL_GetDisplayMode(0, 0, &displayMode);
	SDL_Renderer * displayRenderer;
	displayWindow = SDL_CreateWindow("Sample",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, displayMode.w, displayMode.h, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN);
	
	cout << "Built context with opengl version: " << openglMajorVersion << "." << openglMinorVersion << "\n";
	SDL_GL_CreateContext(displayWindow);
	
	return displayWindow;
}

#endif