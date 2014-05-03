
#pragma once

#include "stdafx.h"

#include "InputEvent.h"
#include "GameData.h"
#include "AsyncTask.h"
#include "SyncTask.h"
#include "GameEvent.h"
#include "MovingAverage.h"

// Callback on iOS to set the render flag
void iOSAnimationCallback(void *context);

// Callback from the event system to allow filtering (sniff for app events)
int EventFilter(void *context, SDL_Event *event);

class LoadingScreen;

class VoxEngine {
	//Entry point for the program
	//should be called once
	static void ProgramEntryPoint();

	//The loading screen for when its needed
	//Constructed during Start()
	static LoadingScreen * load;

	//Do anything necessary to build the render context
	static void BuildRenderContext();

	//Setup the game
	//one time setup which requires
	//an opengl context to be setup
	static void Start();

	//Start the render loop
	//on most platforms
	static void StartRenderLoop();

	//Called over and over while rendering
	static void RenderLoop();

	//Close up and clean up at this point
	static void Exit();

    // Make sure its safe to render
	//if it can't be made safe, returns false to indicate
	//rendering should be skipped
    static bool WaitForSafeRender();
    
	//Build an SDL context
	static SDL_Window* BuildSDLContext(int openglMajorVersion, int openglMinorVersion, float requiredGLSLVersion);

	//Post a frame
	//on most platform this does buffer swap
	static void PostFrame();


	//From a given size find a good size for the 2d interface
	//which mostly fills the screen
	static void ResizeWindow();

	//Read events into a list
	//also act on quit and resize events
	static void ProcessEvents(vector<InputEvent> & events);

	//Becomes false when SDL_Quit is found
	static bool continueGameLoop;

	//The delta between computer time (OS::Now()) and 
	//game simulation time
	static double gameEventDelta;

	//The current time in the game
	static double globalTime;

	//The current size of the window
	static int curWidth;
	static int curHeight;

	//IOS values
	static bool renderingIsSafe;
	static bool iOSRenderRequested;
	static double iOSRenderTime;

	//The scaled size of the 2d interface over the window
	static vec2 scaledSize;
	//The scaling applied to the mouse positions
	static vec2 scaleFactor;

	//An async operation to execute before the next frame
	static AsyncTask * task;

	//Some things require an sdl window
	static SDL_Window * displayWindow;

	friend int main(int argc, char** argv);
    friend int EventFilter(void *context, SDL_Event *event);
    friend void iOSAnimationCallback(void *context);
	friend void doFrame(int width, int height);
public:
	//Run an async task before the next frame switch
	static void SetAsyncTask(AsyncTask * task);

	//If you need to run a task on the main thread
	//do so from here
	static SyncTask SynchronousTask;

	//Saved global game data
	//A good place for things that are per-device
	//instead of per-save
	//Such as options, account data, etc.
	static GameData GlobalSavedData;


	//Get the average update and draw times
	static MovingAverage<float> DrawTime;
	static MovingAverage<float> UpdateTime;

    
    // state changed changed event
    static GameEvent<void (bool)> ApplicationStateChanged;
};