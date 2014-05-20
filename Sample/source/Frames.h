#pragma once

#include "stdafx.h"

#include "ShaderGroup.h"

class GameSystem;

//The fast way of accessing the current system
extern GameSystem * CurrentSystem;

class Frames {

	//A list of the game systems to send input to
	//These systems do all of the actual work
	static int currentSystem; //System starts at 0
	static map<int,GameSystem*> systems;
	static ShaderGroup * shaders;
    
	//Build the initial system list. Should be called by main()
	//before the game starts
	static void BuildSystemList();

	//If during the last sim frame the current system was changed
	//enact the change now
	static void UpdateAliveFrame();

	//So that main can call the above functions
	friend class VoxEngine;
public:
	//The list of current systems
	enum Systems {
		FRAME_MAINMENU,
		FRAME_GAME,
		FRAME_PAUSEMENU,
		FRAME_UPGRADEMENU,
	};
	//Change which game system is receiving input and drawing
	static void SetSystem(Systems system);
	//Change which game system is receiving input and drawing
	static GameSystem * GetSystem(Systems system);
};