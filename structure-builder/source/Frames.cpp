#include "stdafx.h"
#include "Frames.h"
#include "GameSystem.h"
#include "ShaderGroup.h"
//Frames
#include "DesignFrame.h"
#include <thread>

//The current system
GameSystem * CurrentSystem = NULL;

//The integer reference
int Frames::currentSystem;
map<int,GameSystem*> Frames::systems;
bool Frames::loadingComplete;
ShaderGroup * Frames::shaders;

//Build the initial system list. Should be called by main()
void Frames::BuildSystemList() {
	//The shader group builds the shaders now
	//so it can only be built when the opengl context is ready
	shaders = new ShaderGroup();
	cout << "Building systems list...";

	//Build each frame
	systems[FRAME_DESIGNFRAME] = new DesignFrame(shaders);




	//Set initial frame  
	SetSystem(FRAME_DESIGNFRAME);
	//Start deferred loading on another thread
	loadingComplete = false;
	//Thread starts immediately
	thread thr([](){
		//Start each system
		for (auto system : systems)
			system.second->Build();
		//Mark the system list as loaded
		loadingComplete = true;
	});
	//Thread will automatically cleanup on end
	thr.detach();
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
