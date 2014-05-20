#include "stdafx.h"
#include "Frames.h"
#include "GameSystem.h"

//Frames
#include "BaseFrame.h"
#include "MenuFrame.h"
#include "PauseFrame.h"
#include "UpgradeFrame.h"
#include <thread>

//The current system
GameSystem * CurrentSystem = NULL;

//The integer reference
int Frames::currentSystem;
map<int,GameSystem*> Frames::systems;
ShaderGroup * Frames::shaders;

//Build the initial system list. Should be called by main()
void Frames::BuildSystemList() {
	//The shader group builds the shaders now
	//so it can only be built when the opengl context is ready
	shaders = new ShaderGroup();
    
	cout << "Building systems list...";

	//Build each frame
	systems[FRAME_MAINMENU] = new MenuFrame(shaders);
	systems[FRAME_GAME] = new BaseFrame(shaders);
	systems[FRAME_PAUSEMENU] = new PauseFrame(shaders);
	systems[FRAME_UPGRADEMENU] = new UpgradeFrame(shaders);
	//SetSystem(FRAME_GAME);

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
