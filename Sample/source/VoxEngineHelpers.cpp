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

// Should we be rendering?
bool VoxEngine::renderingIsSafe = true;
bool VoxEngine::iOSRenderRequested = false;
double VoxEngine::iOSRenderTime = 0.0;

LoadingScreen * VoxEngine::load;

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

//The SDL display window used by most platforms
SDL_Window * VoxEngine::displayWindow = NULL;

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

//Program entry point
int main(int argc, char** argv)
{

	//Give control to the voxengine immediatly
	VoxEngine::ProgramEntryPoint();
	return 0;
}

void VoxEngine::ResizeWindow()
{
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

	cout << "XXX Internal resize: " << finalResolution.x << "," << finalResolution.y << "\n";
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
				cout << "XXXXXXXXXXXXXX EVENT XXXXXXXXXXXXX\n";
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

// Called by SDL to analyze pumped events
int EventFilter(void *context, SDL_Event *event)
{
    // Figure out what input event this is
    switch (event->type)
    {
#ifdef __MOBILE__
        // If the application will enter the background, it is no longer safe to render
        case SDL_APP_WILLENTERBACKGROUND:
            VoxEngine::renderingIsSafe = false;
            VoxEngine::ApplicationStateChanged.Fire([&](function<void (bool)> subscriber)
            {
                subscriber(VoxEngine::renderingIsSafe);
            });
            return 0;
            break;
            
        // If the application has resumed, it is again safe to render
        case SDL_APP_DIDENTERFOREGROUND:
            VoxEngine::renderingIsSafe = true;
            VoxEngine::ApplicationStateChanged.Fire([&](function<void (bool)> subscriber)
            {
                subscriber(VoxEngine::renderingIsSafe);
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