// VoxEngine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Frames.h"
#include "GameSystem.h"
#include "InterfaceGlobals.h"
#include "OS.h"


SDL_Renderer* displayRenderer;

//A variable which at runtime can be used to figure out what version is running
int OpenglVersion;

//This should be encapsualted in a class, but... here it is
//returns NULL if that context couldn't be constructed
SDL_Window* BuildSDLContext(int openglMajorVersion, int openglMinorVersion, float requiredGLSLVersion);

//Game entry point
int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window * displayWindow = NULL;

#ifndef __MOBILE__
	//Build us a state of the art context
	displayWindow = BuildSDLContext(3,2,1.4f);
	OpenglVersion = 31;
#endif
	//If that fails, try for something less state of the art
	if (displayWindow == NULL) {
		displayWindow = BuildSDLContext(2,0,0.0f);
		OpenglVersion = 20;
	}

	if (displayWindow == NULL) {
		cout << "Failed to open any opengl context on this device. Your device is too out dated, sorry.\n";
		SDL_Quit();
		return -5;
	}
    
	//Start GLEW for windows/linux/OSX
#ifndef __MOBILE__
	glewExperimental = GL_TRUE;
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		cout << glewGetString(GLEW_VERSION) << ", Error: " << glewGetErrorString(res) << "\n";
		return -1;
	}

	//Copied from some glue initiation code
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	if (GLEW_VERSION_3_1)
		cout << "Glew says opengl 3.1 is supported\n";

#endif

	//Setup sensible basics for opengl
	glEnable ( GL_DEPTH_TEST );
	glDepthFunc(GL_LEQUAL);
	glClearColor(.5,.5,.5,1.0);

	//Enable basic alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//re-enable for performance reasons after voxel system complete
	glDisable(GL_CULL_FACE);

	//Attempt to enable vsync (fails on mobile)
	SDL_GL_SetSwapInterval(1);

	//Initialze the dialog constants
	VisualInterface.init();
	//Populate the list of game systems
	Frames::BuildSystemList();

	
	bool continueGameLoop = true;
	//The game loop begins
	while (continueGameLoop) {
		//Determine the width,height of the draw frame
		//also used for unscaling touch events
		int width, height;
		SDL_GetWindowSize(displayWindow,&width,&height);
		vector<InputEvent> eventQueue;
		
		//You can poll up to 20 events per frame
		//we don't want to take all day though
		for (int i = 0; i < 20; i++) {
			//Poll for events
			SDL_Event event;
			int eventPolled = SDL_PollEvent(&event);
		
			if (eventPolled) {
				//Convert sdl event to InputEvent
				switch (event.type) {
				case SDL_KEYDOWN:
					eventQueue.push_back(InputEvent(InputEvent::KeyboardDown,OS::Now(),event.key.keysym.sym));
					break;
				case SDL_KEYUP:
					eventQueue.push_back(InputEvent(InputEvent::KeyboardUp,OS::Now(),event.key.keysym.sym));
					break;
				case SDL_MOUSEBUTTONDOWN:
					eventQueue.push_back(InputEvent(InputEvent::MouseDown,OS::Now(),(float)event.button.x,(float)event.button.y));
					break;
				case SDL_MOUSEBUTTONUP:
					eventQueue.push_back(InputEvent(InputEvent::MouseUp,OS::Now(),(float)event.button.x,(float)event.button.y));
					break;
				case SDL_MOUSEMOTION:
					eventQueue.push_back(InputEvent(InputEvent::MouseMove,OS::Now(),(float)event.motion.x,(float)event.motion.y));
					break;
				case SDL_FINGERMOTION:
					//cout << "MOVED: " << event.tfinger.x << "," << event.tfinger.y << "\n";
					eventQueue.push_back(InputEvent(InputEvent::MouseMove,OS::Now(),event.tfinger.x*width,event.tfinger.y*height));
					break;
				case SDL_FINGERUP:
					eventQueue.push_back(InputEvent(InputEvent::MouseUp,OS::Now(),event.tfinger.x*width,event.tfinger.y*height));
					break;
				case SDL_FINGERDOWN:
					eventQueue.push_back(InputEvent(InputEvent::MouseDown,OS::Now(),event.tfinger.x*width,event.tfinger.y*height));
					break;
				case SDL_QUIT:
					//Stops the next iteration of the game loop
					continueGameLoop = false;
					break;
				}
			}
			else
				//No more events
				break;
		}
			

		//Update the frame simulation (very broken)
		CurrentSystem->Update(0,0,eventQueue);
		//Run the frame draw
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		//Draw
		CurrentSystem->Draw((double)width,(double)height);

		/* Swap our back buffer to the front */
		SDL_GL_SwapWindow(displayWindow);

		//Update the current system
		Frames::UpdateAliveFrame();
	}

	SDL_Quit();
	
	return 0;
}




//returns NULL if that context couldn't be constructed
SDL_Window* BuildSDLContext(int openglMajorVersion, int openglMinorVersion, float requiredGLSLVersion) {
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
#ifndef __ANDROID__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif


	SDL_Window* displayWindow;
	
	SDL_RendererInfo displayRendererInfo;
	displayWindow = SDL_CreateWindow("Sample",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	/*SDL_GetRendererInfo(displayRenderer, &displayRendererInfo);
	if ((displayRendererInfo.flags & SDL_RENDERER_ACCELERATED) == 0 ||
		(displayRendererInfo.flags & SDL_RENDERER_TARGETTEXTURE) == 0) {
		cout << "Failed to build context with opengl version: " << openglMajorVersion << "." << openglMinorVersion << "\n";
		SDL_DestroyWindow(displayWindow);
		return NULL;
	}
	else
		cout << "Built context with opengl version: " << openglMajorVersion << "." << openglMinorVersion << "\n";
     */
	
    SDL_GL_CreateContext(displayWindow);
    
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

	return displayWindow;
}