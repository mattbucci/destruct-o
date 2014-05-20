#include "stdafx.h"
#include "VoxEngine.h"

#ifndef __MOBILE__

//Do anything necessary to build the render context
void VoxEngine::BuildRenderContext() {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

	//Build us a state of the art context
	displayWindow = BuildSDLContext(3,2,1.4f);
	OpenglVersion = 31;
	GLSLVersion = 140;

	if (displayWindow == NULL) {
		cout << "Failed to open any opengl context on this device. Your device is too out dated, sorry.\n";
		SDL_Quit();
		return;
	}

	glewExperimental = GL_TRUE;
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		cout << "GLEW ERRROR: " << glewGetString(GLEW_VERSION) << ", Error: " << glewGetErrorString(res) << "\n";
		SDL_Quit();
		return;
	}

	//Copied from some glue initiation code
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	if (GLEW_VERSION_3_1)
		cout << "Glew says opengl 3.1 is supported\n";

    // Attempt to enable vsync (fails on mobile)
	SDL_GL_SetSwapInterval(1);

    
	/* Print information about attached joysticks (actually works on iOS WOOHOO)*/
	printf("There are %d joystick(s) attached\n", SDL_NumJoysticks());
	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
		const char *name = SDL_JoystickNameForIndex(i);
		printf("Joystick %d: %s\n", i, name ? name : "Unknown Joystick");
		joystick = SDL_JoystickOpen(i);
		if (joystick == NULL) {
			fprintf(stderr, "SDL_JoystickOpen(%d) failed: %s\n", i,
					SDL_GetError());
		} else {
			char guid[64];
			SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joystick),
									  guid, sizeof (guid));
			printf("       axes: %d\n", SDL_JoystickNumAxes(joystick));
			printf("      balls: %d\n", SDL_JoystickNumBalls(joystick));
			printf("       hats: %d\n", SDL_JoystickNumHats(joystick));
			printf("    buttons: %d\n", SDL_JoystickNumButtons(joystick));
			printf("instance id: %d\n", SDL_JoystickInstanceID(joystick));
			printf("       guid: %s\n", guid);
			SDL_JoystickClose(joystick);
		}
	}
	
	// For now, need a selector for mobile since joystick #0 is the built in accelerometer
	if(SDL_NumJoysticks() > 0)
	{
		joystick = SDL_JoystickOpen(0);
		printf("Using joystick = %s\n", SDL_JoystickName(joystick));
	}

	//Get the current window size
	SDL_GetWindowSize(displayWindow,&curWidth,&curHeight);
}


//returns NULL if that context couldn't be constructed
SDL_Window* VoxEngine::BuildSDLContext(int openglMajorVersion, int openglMinorVersion, float requiredGLSLVersion) {
	// Request double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	// Request a 32 bit depth buffer
#if (defined WIN32) || (defined __LINUX__)
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
#else
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
#endif

	
	// Request 32 bit color buffer (RGBA8888)
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	
	// Request OpenGL 3.2
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, openglMajorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, openglMinorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


	SDL_Window* displayWindow;
#ifdef WIN32
	SDL_RendererInfo displayRendererInfo;
	SDL_Renderer * displayRenderer;
	SDL_CreateWindowAndRenderer(1136,640,SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,&displayWindow,&displayRenderer);
	SDL_GetRendererInfo(displayRenderer, &displayRendererInfo);
	if ((displayRendererInfo.flags & SDL_RENDERER_ACCELERATED) == 0 ||
		(displayRendererInfo.flags & SDL_RENDERER_TARGETTEXTURE) == 0) {
		cout << "Failed to build context with opengl version: " << openglMajorVersion << "." << openglMinorVersion << "\n";
		SDL_DestroyWindow(displayWindow);
		return NULL;
	}
#else
	displayWindow = SDL_CreateWindow("Destructo",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, 1136, 640, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
#endif
	
	cout << "Built context with opengl version: " << openglMajorVersion << "." << openglMinorVersion << "\n";
	SDL_GL_CreateContext(displayWindow);
	
	//Setup the icon
	SDL_SetWindowIcon(displayWindow,SDL_LoadBMP("DestructoIcon.bmp"));
	SDL_SetWindowTitle(displayWindow,"Destruct-o");

	//Get glsl version
	char * versionString = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	//This function can fail to return a proper version string
	//in a bunch of different annoying and creative ways. (Even on computers
	//with good GLSL versions)
	//If the user specified 0 as the requirement, don't even bother checking
	if ((requiredGLSLVersion <= 0) || (versionString == NULL) || (strlen(versionString) <= 0) || (atof(versionString) <= 0))
		cout << "GLSL version check failed (or was skipped), lets just hope we have a recent enough one...\n";
	else {
		//Verify the version is correct
		float version = (float)atof(versionString);
		cout << "Detected GLSL version: " << versionString << " [" << version << "]\n";
		if (version <= requiredGLSLVersion) {
			//Fail out, you go tthe opengl version you needed, but not glsl version
			cout << "GLSL version is insufficient for this opengl context (needs at least " << requiredGLSLVersion << ")\n";
			SDL_DestroyWindow(displayWindow);
			return NULL;
		}
	}

	glEnable(GL_MULTISAMPLE);

	return displayWindow;
}


#endif