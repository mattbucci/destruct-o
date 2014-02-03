// VoxEngine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Frames.h"
#include "GameSystem.h"
#include "InterfaceGlobals.h"
#include "OS.h"

SDL_Renderer* displayRenderer;


//SDL defines main for some reason. This is undesirable
#undef main

//Game entry point
int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_VIDEO);

	//FROM: http://www.opengl.org/wiki/Tutorial1:_Creating_a_Cross_Platform_OpenGL_3.2_Context_in_SDL_(C_/_SDL)
	/* Request opengl 3.2 context.
	 * SDL doesn't have the ability to choose which profile at this time of writing,
	 * but it should default to the core profile */
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
 
	/* Turn on double buffering with a 24bit Z buffer.
	 * You may need to change this to 16 or 32 for your system */
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);


	SDL_Window* displayWindow;
	
	SDL_RendererInfo displayRendererInfo;
	SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN, &displayWindow, &displayRenderer);
	SDL_GetRendererInfo(displayRenderer, &displayRendererInfo);
	/*TODO: Check that we have OpenGL */
	if ((displayRendererInfo.flags & SDL_RENDERER_ACCELERATED) == 0 || 
		(displayRendererInfo.flags & SDL_RENDERER_TARGETTEXTURE) == 0) {
		/*TODO: Handle this. We have no render surface and not accelerated. */
		cout << "Something went terribly wrong";
	}
	
	/* Create our opengl context and attach it to our window */
	SDL_GLContext maincontext = SDL_GL_CreateContext(displayWindow);
	
	//Start GLEW for windows 32
#ifdef WIN32
	glewExperimental = GL_TRUE; 
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		cout << glewGetString(GLEW_VERSION) << ", Error: " << glewGetErrorString(res) << "\n";
		return -1;
	}

	//Copied from some glue initiation code
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	//Setup sensible basics for opengl
	glEnable ( GL_DEPTH_TEST );
	glDepthFunc(GL_LEQUAL);

	//Enable basic alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//re-enable for performance reasons after voxel system complete
	glDisable(GL_CULL_FACE);

	//Attempt to enable vsync
	SDL_GL_SetSwapInterval(1);

	//Initialze the dialog constants
	VisualInterface.init();
	//Populate the list of game systems
	Frames::BuildSystemList();

	glClearColor(0.0f,0.0f,0.0f,1.0f);

	//The game loop begins
	while (1) {
		//Poll for events
		SDL_Event event;
		int eventPolled = SDL_PollEvent(&event);
		//Right now this is rather wastefull, rebuild this event queue every time
		vector<InputEvent> eventQueue;
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
				eventQueue.push_back(InputEvent(InputEvent::MouseDown,OS::Now(),event.button.x,event.button.y));
				break;
			case SDL_MOUSEBUTTONUP:
				eventQueue.push_back(InputEvent(InputEvent::MouseUp,OS::Now(),event.button.x,event.button.y));
				break;
			case SDL_MOUSEMOTION:
				eventQueue.push_back(InputEvent(InputEvent::MouseMove,OS::Now(),event.motion.x,event.motion.y));
				break;
			}
			//We do this outside the switch statement so we can break out of the while loop
			if (event.type == SDL_QUIT)
				break;
		}
			

		//Update the frame simulation (very broken)
		CurrentSystem->Update(0,0,eventQueue);
		//Run the frame draw
		glClearColor ( 1.0, 0.0, 0.0, 1.0 );
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		CurrentSystem->Draw(800,600);
		/* Swap our back buffer to the front */
		SDL_GL_SwapWindow(displayWindow);

		//Update the current system
		Frames::UpdateAliveFrame();
	}

	SDL_Quit();
	
	return 0;
}

