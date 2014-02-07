#include "stdafx.h"
#include "Frames.h"
#include "GameSystem.h"
#include "ShaderGroup.h"
//Frames
#include "BaseFrame.h"

//The current system
GameSystem * CurrentSystem = NULL;

//The integer reference
int Frames::currentSystem;
map<int,GameSystem*> Frames::systems;

//Build the initial system list. Should be called by main()
void Frames::BuildSystemList() {
	//TODO: Move this to the class so it is claned up properly
	ShaderGroup * shaders = new ShaderGroup();

	cout << "Building systems list...";

	//Build each frame
	systems[FRAME_MAINMENU] = new BaseFrame(shaders);
	//Set initial frame  
	SetSystem(FRAME_MAINMENU);
}
//If during the last sim frame the current system was changed
//enact the change now
void Frames::UpdateAliveFrame() {
	if (CurrentSystem != systems[currentSystem]) {
		CurrentSystem->OnFrameLeave();
		CurrentSystem = systems[currentSystem];
		CurrentSystem->OnFrameFocus();
	}
}


void Frames::SetSystem(Systems system) {
	currentSystem = system;
	//Only enact the change immediately if there is no running frame currently
	if (CurrentSystem == NULL) {
		CurrentSystem = systems[system];
		CurrentSystem->OnFrameFocus();
	}
}

GameSystem * Frames::GetSystem(Systems system) {
	return systems[system];
}
