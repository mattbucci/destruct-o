// VoxEngine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Frames.h"
#include "GameSystem.h"
#include "InterfaceGlobals.h"
#include "OS.h"
#include "VoxEngine.h"
#include "LoadingScreen.h"


SDL_Renderer* displayRenderer;

//A variable which at runtime can be used to figure out what version is running
int OpenglVersion;

//Static values within the VoxEngine class
//Becomes false when SDL_Quit is found
bool VoxEngine::continueGameLoop;

//The delta between computer time (OS::Now()) and 
//game simulation time
double VoxEngine::gameEventDelta;

//The current time in the simulation loop
//always starts at 0.0
double VoxEngine::gameSimulationTime = 0.0;

//The current size of the window
int VoxEngine::curWidth;
int VoxEngine::curHeight;

//Program entry point
int main(int argc, char** argv)
{
	VoxEngine::Start();
	return 0;
}

//Game entry point
void VoxEngine::Start() {
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
		
#ifdef __IPHONEOS__
		//For iOS, globally force the newer opengl version
		OpenglVersion = 31;
#else
		OpenglVersion = 20;
#endif
	}

	if (displayWindow == NULL) {
		cout << "Failed to open any opengl context on this device. Your device is too out dated, sorry.\n";
		SDL_Quit();
		return;
	}
	
	//Start GLEW for windows/linux/OSX
#ifndef __MOBILE__
	glewExperimental = GL_TRUE;
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		cout << glewGetString(GLEW_VERSION) << ", Error: " << glewGetErrorString(res) << "\n";
		SDL_Quit();
		return;
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
	//Do game initial load
	//at this point we're going to enter a really fast draw loop
	//and show our please wait dialog
	{
		LoadingScreen load;
		//Assume one of the frames constructed the 2d shader
		GL2DProgram * shader = (GL2DProgram*)Frames::shaders->GetShader("2d");

		glActiveTexture(GL_TEXTURE0);

		while (!Frames::loadingComplete) {
			//Run the frame draw
			glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			//Pump events even though we ignore them
			SDL_PumpEvents();
			//Check window size
			SDL_GetWindowSize(displayWindow,&curWidth,&curHeight);
			//render the loading screen
			glViewport(0, 0, curWidth, curHeight);
			load.Draw(curWidth,curHeight,shader);
			SDL_GL_SwapWindow(displayWindow);
		}
	}


	//Start up game
	continueGameLoop = true;
	//Get the current window size
	SDL_GetWindowSize(displayWindow,&curWidth,&curHeight);

	//Mark the simulation starting time
	//Start the simulation one update loop into the past
	//this gurantees at least one update is run before the first Draw()
	gameSimulationTime = 0;
	gameEventDelta = gameSimulationTime-OS::Now();
	gameSimulationTime = -SIMULATION_TIME;

	//The game loop begins
	while (continueGameLoop) {

		//Jump ahead detection, if the simulation loop
		//is behind more than 2 seconds, assume the application
		//was paused for some reason (break point)
		//and jump the time up, so that it seems like no time passed
		//at all
		if (((OS::Now()+gameEventDelta) - gameSimulationTime) >= 2.0) {
			//Do jump ahead
			cout << "Note: performed jumpahead from " << gameEventDelta;
			gameEventDelta = gameSimulationTime-OS::Now();
			cout << " to " << gameEventDelta << "\n";
		}

		//While you're behind on physics frames catch up
		//If you become more behind during calculation
		//we continue anyways, so that frames are drawn sometimes
		//even if they fall behind a bit
		double timeDifference = (OS::Now()+gameEventDelta)-gameSimulationTime;
		while (timeDifference >= SIMULATION_TIME) {
			timeDifference -= SIMULATION_TIME;
			//Run the simulation
			//Read events
			vector<InputEvent> eventQueue;
			ProcessEvents(eventQueue);
			//Simulate actions
			CurrentSystem->Update(SIMULATION_TIME,gameSimulationTime,eventQueue);
			//Update the simulation time
			gameSimulationTime += SIMULATION_TIME;
		}

		//Run the frame draw
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		//Draw
		CurrentSystem->Draw((double)curWidth,(double)curHeight);

		/* Swap our back buffer to the front */
		SDL_GL_SwapWindow(displayWindow);
	

		//Update the current system selection
		//if a swap was requested during one of the updates
		//it will happen now
		Frames::UpdateAliveFrame();
	}

	SDL_Quit();
}

void VoxEngine::ProcessEvents(vector<InputEvent> & eventQueue) {
		//You can poll up to 20 events per frame
		//we don't want to take all day though
		for (int i = 0; i < 20; i++) {
			//Poll for events
			SDL_Event event;
			int eventPolled = SDL_PollEvent(&event);
		
			if (eventPolled) {
				//Convert sdl event to InputEvent
				switch (event.type) {
				//Handle input events
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
					//Add in the relative motion information
					eventQueue.back().RelX = (float)event.motion.xrel;
					eventQueue.back().RelY = (float)event.motion.yrel;
					break;
				case SDL_FINGERMOTION:
					//cout << "MOVED: " << event.tfinger.x << "," << event.tfinger.y << "\n";
					eventQueue.push_back(InputEvent(InputEvent::MouseMove,OS::Now(),event.tfinger.x*curWidth,event.tfinger.y*curHeight));
					break;
				case SDL_FINGERUP:
					eventQueue.push_back(InputEvent(InputEvent::MouseUp,OS::Now(),event.tfinger.x*curWidth,event.tfinger.y*curHeight));
					break;
				case SDL_FINGERDOWN:
					eventQueue.push_back(InputEvent(InputEvent::MouseDown,OS::Now(),event.tfinger.x*curWidth,event.tfinger.y*curHeight));
					break;
				//Handle events which directly effect the operation of the game engine
				case SDL_WINDOWEVENT: 
					//Window events have their own set of things that could happen
					switch (event.window.event) {
					case SDL_WINDOWEVENT_RESIZED:
						curWidth = event.window.data1;
						curHeight = event.window.data2;
						glViewport(0,0,curWidth,curHeight);
						break;
					}
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
}


//returns NULL if that context couldn't be constructed
SDL_Window* VoxEngine::BuildSDLContext(int openglMajorVersion, int openglMinorVersion, float requiredGLSLVersion) {
	// Request double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	// Request a 32 bit depth buffer
#if (defined __ANDROID__) || (defined WIN32) || (defined __LINUX__)
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
#ifndef __MOBILE__
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif


	SDL_Window* displayWindow;
#if (defined __MOBILE__)
	// On mobile, create a fullscreen display the size of the device's screen
	SDL_DisplayMode displayMode;
	SDL_GetDisplayMode(0, 0, &displayMode);
	displayWindow = SDL_CreateWindow("Sample",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, displayMode.w, displayMode.h, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN);
#elif (defined WIN32)
	SDL_RendererInfo displayRendererInfo;
	SDL_CreateWindowAndRenderer(800,600,SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,&displayWindow,&displayRenderer);
	SDL_GetRendererInfo(displayRenderer, &displayRendererInfo);
	if ((displayRendererInfo.flags & SDL_RENDERER_ACCELERATED) == 0 ||
		(displayRendererInfo.flags & SDL_RENDERER_TARGETTEXTURE) == 0) {
		cout << "Failed to build context with opengl version: " << openglMajorVersion << "." << openglMinorVersion << "\n";
		SDL_DestroyWindow(displayWindow);
		return NULL;
	}
#else
	displayWindow = SDL_CreateWindow("Sample",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
#endif
	
	cout << "Built context with opengl version: " << openglMajorVersion << "." << openglMinorVersion << "\n";
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
