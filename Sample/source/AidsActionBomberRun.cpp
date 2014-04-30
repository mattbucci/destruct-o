#include "stdafx.h"
#include "AidsActionBomberRun.h"
#include "BaseFrame.h"

#include "ActorAI.h"

CLASS_SAVE_CONSTRUCTOR(AidsActionBomberRun);

//This constructor is /only/ called by the loader
AidsActionBomberRun::AidsActionBomberRun() : AidsAction(vec3(),0) {
	//Loader runs directly after this
}

//Constructing the action should start it
AidsActionBomberRun::AidsActionBomberRun(vec3 actionLocation) : AidsAction(actionLocation,GetIntensityValue()) {
	//Find a hidden location
	vec3 deplyLocation = findHiddenLocation(actionLocation,3);
	//Deploy
	deployed = Game()->Actors.BuildAI(deplyLocation,"bomber.json");
}

//Every subclass should have a static member GetIntensityValue()
float AidsActionBomberRun::GetIntensityValue() {
	return 40.0f;
}


//If this returns true clean up the action
//the action has ended
bool AidsActionBomberRun::Update() {
	//If they're dead, they're dead
	if (deployed->Dead())
		return true;
	//If they're too far away, undeploy them
	//This takes care of bombers after bombing runs automatically
	if (glm::distance(deployed->GetPosition(),Game()->Actors.Player()->GetPosition()) > 300) {
		deployed->Kill();
	}
	return false;
}