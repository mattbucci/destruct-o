
#pragma once

#include "stdafx.h"

#include "InputEvent.h"


#define SIMULATION_TIME .01

class VoxEngine {
	//Entry point for the game engine
	static void Start();
	//Build an SDL context
	static SDL_Window* BuildSDLContext(int openglMajorVersion, int openglMinorVersion, float requiredGLSLVersion);

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

	//The current time in the simulation loop
	//always starts at 0.0
	static double gameSimulationTime;

	//The current size of the window
	static int curWidth;
	static int curHeight;

	//The scaled size of the 2d interface over the window
	static vec2 scaledSize;
	//The scaling applied to the mouse positions
	static vec2 scaleFactor;

	friend int main(int argc, char** argv);
public:
	//This gets the update cycle time
	static double GetGameSimTime();
};