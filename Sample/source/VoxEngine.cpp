// VoxEngine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Frames.h"
#include "GameSystem.h"
#include "InterfaceGlobals.h"
#include "OS.h"
#include "VoxEngine.h"
#include "LoadingScreen.h"
#include "BaseFrame.h"

SDL_Renderer* displayRenderer;

// Should we be rendering?
bool renderingIsSafe = true;
bool iOSRenderRequested = false;
double iOSRenderTime = 0.0;

//A variable which at runtime can be used to figure out what version is running
int OpenglVersion;
//Used to determine the GLSL version used
//supports only two values 110 and 140
int GLSLVersion;

// The current joystick
SDL_Joystick *joystick = NULL;

//Static values within the VoxEngine class
//Becomes false when SDL_Quit is found
bool VoxEngine::continueGameLoop;

//The delta between computer time (OS::Now()) and 
//game simulation time
double VoxEngine::gameEventDelta;

//The current time in the simulation loop
//always starts at 0.0
double VoxEngine::globalTime = 0.0;

//The current size of the window
int VoxEngine::curWidth;
int VoxEngine::curHeight;

//The scaled size of the 2d interface over the window
vec2 VoxEngine::scaledSize;
//The scaling applied to the mouse positions
vec2 VoxEngine::scaleFactor;

//An async operation to execute before the next frame
AsyncTask * VoxEngine::task = NULL;

//If you need to run a task on the main thread
//do so from here
SyncTask VoxEngine::SynchronousTask;

//Get the average update and draw times
MovingAverage<float> VoxEngine::DrawTime(20);
MovingAverage<float> VoxEngine::UpdateTime(20);


//Save data to keep across all games
GameData VoxEngine::GlobalSavedData;

GameEvent<void (bool)> VoxEngine::ApplicationStateChanged;

/*
//For heap testing on windows. Is actually very useful
int heapHook(int nAllocType, void *pvData,
      size_t nSize, int nBlockUse, long lRequest,
      const unsigned char * szFileName, int nLine ) {
	int x = 5;
	return 1;
}
*/

//Program entry point
int main(int argc, char** argv)
{
	//_CrtSetAllocHook(heapHook);
	//VoxEngine::Start(640,480);
	VoxEngine::DoInit();
	return 0;
}

bool initialized = false;

void doFrame(int width, int height) {
	VoxEngine::Start(width,height);
}

void VoxEngine::DoInit() {

	//Start our version of GLEW for android
#ifdef __ANDROID__
	//Let the android GLEW decide which opengl version to act like
	OpenglVersion = initAndroidGlew();
	if (OpenglVersion > 0)
		cout << "AndroidGLEW loaded successfully\n";
	else {
		cout << "Failed to load some necessary extensions for android glew\n";
		cout << "PROGRAM IS NOW EXITING\n";
		return;
	}
		
#endif
	cout << "Rendering one frame with size: " << curWidth << "," << curHeight << "\n";
	//Randomize you're starting time
	//it's not perfect, but should be sufficient
	srand (time(NULL));

	//Setup sensible basics for opengl
	glEnable ( GL_DEPTH_TEST );
	glDepthFunc(GL_LEQUAL);
	glClearColor(.5,.5,.5,1.0);

	//Enable basic alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//re-enable for performance reasons after voxel system complete
	glDisable(GL_CULL_FACE);

	
   
	// Add the event filter for application events such as pause
	//SDL_AddEventWatch(EventFilter, NULL);
    

	cout << "Using an external opengl version of: " << (char*)glGetString(GL_VERSION) << "\n";
	cout << "Using internal opengl version of: " << OpenglVersion << "\n";
	vector<unsigned char> test;
	lodepng::load_file(test,"loading.png");
	if (test.size() > 0)
		cout << "LOADING LOADED\n";

	cout << "READ FINISHED\n";
	//Initialze the dialog constants
	VisualInterface.init();
	//Populate the list of game systems
	Frames::BuildSystemList();

	//Start up game
	continueGameLoop = true;
    
	//Get the current window size
	//SDL_GetWindowSize(displayWindow,&curWidth,&curHeight);
	curWidth = 640;
	curHeight = 480;
	ResizeWindow();
	glViewport(0, 0, curWidth, curHeight);

	//Mark the simulation starting time
	//Start the simulation one update loop into the past
	//this gurantees at least one update is run before the first Draw()
	globalTime = 0;
	gameEventDelta = globalTime-OS::Now();
	globalTime = -SIMULATION_DELTA;

	//First task is to build the game
	VoxEngine::task = new AsyncTask([]() {Game()->Build();});
	
	
}

//Game entry point
void VoxEngine::Start(int width, int height) {
	//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	//return;


	if (width != curWidth || height != curHeight) {
		curWidth = width;
		curHeight = height;
		ResizeWindow();
		glViewport(0, 0, curWidth, curHeight);
	}


	//The game loop begins
	//while (continueGameLoop)
    //{
        // Wait until its safe to render
        //VoxEngine::WaitForSafeRender();
        
        // If we have a task needing completed (mostly frame switched)
        if(VoxEngine::task != NULL)
        {
			//Start the task
			VoxEngine::task->Start();

			//Assume one of the frames constructed the 2d shader
			GL2DProgram * shader = (GL2DProgram*)Frames::shaders->GetShader("2d");
			LoadingScreen load;

			glActiveTexture(GL_TEXTURE0);
			SynchronousTask.PollingThreadStart();

			while(!VoxEngine::task->IsDone())
            {
				// Wait until its safe to render
                VoxEngine::WaitForSafeRender();
                
                    // Draw the loading screen
                    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
                    load.Draw(curWidth,curHeight,shader);
                    //SDL_GL_SwapWindow(displayWindow);
				//If there's a sync task, execute it now
				SynchronousTask.PollingThreadPoll();

				//Sleep softly and use no cpu power
				OS::SleepTime(1.0/60.0);
			}

			SynchronousTask.PollingThreadStop();

			//Cleanup the task
			delete VoxEngine::task;
			VoxEngine::task = NULL;
		}
		//Jump ahead detection, if the simulation loop
		//is behind more than 2 seconds, assume the application
		//was paused for some reason (break point)
		//and jump the time up, so that it seems like no time passed
		//at all
		if (((OS::Now()+gameEventDelta) - globalTime) >= 2.0) {
			//Do jump ahead
			cout << "Note: performed jumpahead from " << gameEventDelta;
			gameEventDelta = globalTime-OS::Now();
			cout << " to " << gameEventDelta << "\n";
		}

		//For profile debugging
		double updateStartTime = OS::Now();

		//While you're behind on physics frames catch up
		//If you become more behind during calculation
		//we continue anyways, so that frames are drawn sometimes
		//even if they fall behind a bit
		double timeDifference = (OS::Now()+gameEventDelta)-globalTime;
        double physicsFrameTime = OS::Now();
		while (timeDifference >= SIMULATION_DELTA) {
			timeDifference -= SIMULATION_DELTA;
			//Run the simulation
			//Read events
			vector<InputEvent> eventQueue;
			ProcessEvents(eventQueue);
			//Simulate actions
			CurrentSystem->Update(eventQueue);
			//Update the simulation time
			globalTime += SIMULATION_DELTA;
			CurrentSystem->simTime += SIMULATION_DELTA;
		}

		UpdateTime.AddSample(OS::Now()-updateStartTime);

        // Pump Events (iOS vsync is scheduled on the event pump)
        //SDL_PumpEvents();
        
        // If rendering is safe
        if(renderingIsSafe)
        {

				//For debug profiling
				double drawStartTime = OS::Now();

                //Run the frame draw
                glClear ( CurrentSystem->ClearBits() );
                
                //Draw
                CurrentSystem->Draw(scaledSize.x,scaledSize.y);
                
				//Time measured
				DrawTime.AddSample(OS::Now() - drawStartTime);

                /* Swap our back buffer to the front */
                //SDL_GL_SwapWindow(displayWindow);
        }

		//Update the current system selection
		//if a swap was requested during one of the updates
		//it will happen now
		Frames::UpdateAliveFrame();
	//}
}

// Wait until its safe to render (pumps events and returns if safe)
void VoxEngine::WaitForSafeRender()
{
    // Process events
    SDL_PumpEvents();
    
    // Loop if rendering is not safe
    while(!renderingIsSafe)
    {
        // Wait a frame and pump events
        SDL_PumpEvents();
        OS::SleepTime(1.0/60.0);
    }
}

void VoxEngine::ResizeWindow()
{
	cout << "Resized to: " << curWidth << "," << curHeight << "\n";
	const static vec2 targetResolution = vec2(800,600);

	vec2 newSize = vec2((float)curWidth,(float)curHeight);

	//Use the current width/height to find the closest width/height to the 
	//target resolution which maintains aspect ratio
	vec2 currentResolution = newSize;
	currentResolution /= targetResolution;
	vec2 finalResolution;
	if (currentResolution.x > currentResolution.y) {
		//The screen is more wide than 800 by 600
		//Scale the height to fill the screen and then determine the correct width
		finalResolution.y = targetResolution.y;
		finalResolution.x = newSize.x/currentResolution.y;
	}
	else {
		//the screen is more tall than 800 by 600
		//Scale the width to fill the screen and then determine the correct height
		finalResolution.x = targetResolution.x;
		finalResolution.y = newSize.y/currentResolution.x;
	}

	//Save calculated size information
	scaledSize = finalResolution;
	scaleFactor = scaledSize/newSize;
}

void VoxEngine::ProcessEvents(vector<InputEvent> & eventQueue) {
		//You can poll up to 20 events per frame
		//we don't want to take all day though
		for (int i = 0; i < 20; i++) {
			//Poll for events
			SDL_Event event;
			int eventPolled = SDL_PollEvent(&event);
		
			if (eventPolled)
            {
				//Convert sdl event to InputEvent
				switch (event.type)
				{
					// Handle input events
#ifdef __MOBILE__
                    // Mobile only responds to finger events
					case SDL_FINGERMOTION:
						//cout << "MOVED (" << event.tfinger.fingerId << ") : " << event.tfinger.x << "," << event.tfinger.y << "\n";
						eventQueue.push_back(InputEvent(InputEvent::MouseMove,OS::Now(),scaleFactor.x*event.tfinger.x*curWidth,scaleFactor.y*event.tfinger.y*curHeight,(int) event.tfinger.fingerId));
						break;
					case SDL_FINGERUP:
						eventQueue.push_back(InputEvent(InputEvent::MouseUp,OS::Now(),scaleFactor.x*event.tfinger.x*curWidth,scaleFactor.y*event.tfinger.y*curHeight,(int) event.tfinger.fingerId));
						break;
					case SDL_FINGERDOWN:
						eventQueue.push_back(InputEvent(InputEvent::MouseDown,OS::Now(),scaleFactor.x*event.tfinger.x*curWidth,scaleFactor.y*event.tfinger.y*curHeight,(int) event.tfinger.fingerId));
						break;
#else
						// Desktop (well, whatever non mobile is called these days) only responds to keyboard and mouse events
					case SDL_MOUSEBUTTONDOWN:
						eventQueue.push_back(InputEvent(InputEvent::MouseDown,OS::Now(),scaleFactor.x*(float)event.button.x,scaleFactor.y*(float)event.button.y, event.button.button));
						break;
					case SDL_MOUSEBUTTONUP:
						eventQueue.push_back(InputEvent(InputEvent::MouseUp,OS::Now(),scaleFactor.x*(float)event.button.x,scaleFactor.y*(float)event.button.y, event.button.button));
						break;
					case SDL_MOUSEMOTION:
						eventQueue.push_back(InputEvent(InputEvent::MouseMove,OS::Now(),scaleFactor.x*(float)event.motion.x,scaleFactor.y*(float)event.motion.y, event.button.button));
						//Add in the relative motion information
						eventQueue.back().RelX = (float)event.motion.xrel;
						eventQueue.back().RelY = (float)event.motion.yrel;
						break;
#endif
					case SDL_KEYDOWN:
						eventQueue.push_back(InputEvent(InputEvent::KeyboardDown,OS::Now(),event.key.keysym.sym));
						break;
					case SDL_KEYUP:
						eventQueue.push_back(InputEvent(InputEvent::KeyboardUp,OS::Now(),event.key.keysym.sym));
						break;
					
					//Handle events which directly effect the operation of the game engine
					case SDL_WINDOWEVENT: 
						//Window events have their own set of things that could happen
						switch (event.window.event) {
						case SDL_WINDOWEVENT_RESIZED:
							curWidth = event.window.data1;
							curHeight = event.window.data2;
							glViewport(0,0,curWidth,curHeight);
							ResizeWindow();
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
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
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
	displayWindow = SDL_CreateWindow("Destructo",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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

#ifndef __MOBILE__
	glEnable(GL_MULTISAMPLE);
#endif

	return displayWindow;
}

//Run an async task before the next frame switch
void VoxEngine::SetAsyncTask(AsyncTask * task) {
	//If you already have a task ignore the given task
	if (VoxEngine::task != NULL) {
		delete task;
		return;
	}
	//Otherwise set it to your new task
	VoxEngine::task = task;
}

// Called by the iOS animation callback
void iOSAnimationCallback(void *context)
{
    iOSRenderRequested = true;
    iOSRenderTime = OS::Now();
}

// Called by SDL to analyze pumped events
int EventFilter(void *context, SDL_Event *event)
{
    // Figure out what input event this is
    switch (event->type)
    {
#ifdef __MOBILE__
        // If the application will enter the background, it is no longer safe to render
        case SDL_APP_WILLENTERBACKGROUND:
            renderingIsSafe = false;
            VoxEngine::ApplicationStateChanged.Fire([&](function<void (bool)> subscriber)
            {
                subscriber(renderingIsSafe);
            });
            return 0;
            break;
            
        // If the application has resumed, it is again safe to render
        case SDL_APP_DIDENTERFOREGROUND:
            renderingIsSafe = true;
            VoxEngine::ApplicationStateChanged.Fire([&](function<void (bool)> subscriber)
            {
                subscriber(renderingIsSafe);
            });
            return 0;
            break;
#endif
        default:
            break;
    }
    
    // If we don't handle the event, return 1
    return 1;
}