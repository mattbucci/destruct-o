
#pragma once

#include "stdafx.h"

#include "InputEvent.h"


#define SIMULATION_TIME .01

class VoxEngine {
	//Entry point for the game engine
	static void Start();
	//Build an SDL context
	static SDL_Window* BuildSDLContext(int openglMajorVersion, int openglMinorVersion, float requiredGLSLVersion);

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

	friend int main(int argc, char** argv);
public:
	//This gets the update cycle time
	static double GetGameSimTime();
};