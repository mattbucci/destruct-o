#include "stdafx.h"
#include "Frames.h"
#include "GameSystem.h"
#include "InterfaceGlobals.h"
#include "OS.h"
#include "VoxEngine.h"
#include "LoadingScreen.h"
#include "BaseFrame.h"

double physicsFrameTime = 0.0;

//Entry point for the program
//should be called once
void VoxEngine::ProgramEntryPoint()
{
	cout << "Program entry point\n";
	//curWidth/curHeight should be set by android /before/ this point
	//and set by everyone else during BuildRenderContext()


	//You need a render context
	BuildRenderContext();
	//Start the one time setup
	//on most platforms Start() also handles calling the RenderLoop()
	//On any other platform, RenderLoop() must be called manually
	Start();

	//Begin the render loop on platforms which
	//require a direct call (most of them)
	StartRenderLoop();

	//Exit is handled as an event
}

//Setup the game
//one time setup which requires
//an opengl context to be setup
void VoxEngine::Start() {
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
    SDL_AddEventWatch(EventFilter, NULL);

	//Assume an opengl system already setup
	cout << "Using an external opengl version of: " << (char*)glGetString(GL_VERSION) << "\n";
	cout << "Using internal opengl version of: " << OpenglVersion << "\n";
	
	//Randomize you're starting time
	//it's not perfect, but should be sufficient
	srand (time(NULL));

	//Initialze the dialog constants
	VisualInterface.init();
	//Populate the list of game systems
	Frames::BuildSystemList();
	//Load game settings and account data from disk
	SavedDeviceData.Load();
	SavedAccountData.Load();

	//Start up game
	continueGameLoop = true;
    
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

	//Setup load
	load = new LoadingScreen();
}

void VoxEngine::StartRenderLoop() {
	//Android calls RenderLoop() externally
#ifndef __ANDROID__
	while (continueGameLoop) {
		//Render/Update for one frame
		RenderLoop();

		//While rendering isn't safe, sleep and pump events
		//also checks if any event is going to wake you up
		while (!renderingIsSafe) {
			SDL_PumpEvents();
			OS::SleepTime(1.0/60.0);
		}
	}
#endif
}


//Post a frame
//on most platform this does buffer swap
void VoxEngine::PostFrame() {
#ifndef __ANDROID__
	SDL_GL_SwapWindow(displayWindow);
#endif
#ifdef __IPHONEOS__
    iOSRenderRequested = false;
#endif
}

bool doOnce = true;

//Called over and over while rendering
void VoxEngine::RenderLoop() {
    // If we have a task needing completed (mostly frame switched)
    if (VoxEngine::task != NULL) {
		if (!VoxEngine::task->IsStarted()) {
			//Clear progress
			LoadProgress.Update("");
			//Start the task
			VoxEngine::task->Start();

			glActiveTexture(GL_TEXTURE0);
			SynchronousTask.PollingThreadStart();
		}
		else {
			if (!VoxEngine::task->IsDone()) {
				// Wait until its safe to render
				if(VoxEngine::WaitForSafeRender())
				{
					//Assume one of the frames constructed the 2d shader
					GL2DProgram * shader = (GL2DProgram*)Frames::shaders->GetShader("2d");
					// Draw the loading screen
					glClearColor(33.0f/255*.6f,196.0/255*.6f,0,1.0f);
					glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
					load->Draw(scaledSize.x,scaledSize.y,shader);
					//Draw the frame
					PostFrame();
				}

				//If there's a sync task, execute it now
				SynchronousTask.PollingThreadPoll();

				//Run events, process resize events still
				//ignore all else
				for (int i = 0; i < 20; i++) {
					//Poll for events
					SDL_Event event;
					int eventPolled = SDL_PollEvent(&event);
					if (eventPolled && (event.type == SDL_WINDOWEVENT) && (event.window.event == SDL_WINDOWEVENT_RESIZED)) {
						//process resize
						curWidth = event.window.data1;
						curHeight = event.window.data2;
						glViewport(0,0,curWidth,curHeight);
						ResizeWindow();
					}
				}
				//Sleep softly and use no cpu power
				OS::SleepTime(1.0/60.0);
			}
			else {
				//Finish the task
				SynchronousTask.PollingThreadStop();

				//Cleanup the task
				delete VoxEngine::task;
				VoxEngine::task = NULL;
			}
		}
		return;
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
    physicsFrameTime = OS::Now();
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
    SDL_PumpEvents();
        
    // If rendering is safe
    if(VoxEngine::WaitForSafeRender()) {
		//For debug profiling
		double drawStartTime = OS::Now();

        //Run the frame draw
        glClear ( CurrentSystem->ClearBits() );
                
        //Draw
        CurrentSystem->Draw(scaledSize.x,scaledSize.y);
                
		//Time measured
		DrawTime.AddSample(OS::Now() - drawStartTime);

		//Draw the frame
		PostFrame();
    }

	//Update the current system selection
	//if a swap was requested during one of the updates
	//it will happen now
	Frames::UpdateAliveFrame();
	doOnce = false;
}

//Close up and clean up at this point
void VoxEngine::Exit() {

}

// Make sure its safe to render
//if it can't be made safe, returns false to indicate
//rendering should be skipped
bool VoxEngine::WaitForSafeRender()
{
    // Process events
    SDL_PumpEvents();
    
    // If rendering isn't safe, don't do it
    if(!renderingIsSafe)
    {
        // Wait a frame and pump events
        OS::SleepTime(1.0/60.0);
		return false;
    }

#ifdef __IPHONEOS__
	if (!iOSRenderRequested) {
        // Get whichever is shorter (remaining time until next frame render or physics cycle, 1/60 should be replaced with target framerate)
        double sleepTime = glm::min(SIMULATION_DELTA - (OS::Now() - physicsFrameTime), (1.0/60.0) - (OS::Now() - iOSRenderTime));
                
        // Sleep for the minimum amount of time
        if(sleepTime > 0.0)
			OS::SleepTime(sleepTime);
		//Not ready for rendering still
		return false;
	}
#endif
	//It's safe
	return true;
}
