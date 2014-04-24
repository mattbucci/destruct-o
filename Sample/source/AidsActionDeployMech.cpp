#include "stdafx.h"
#include "AidsActionDeployMech.h"
#include "BaseFrame.h"

#include "ActorAIMech.h"

CLASS_SAVE_CONSTRUCTOR(AidsActionDeployMech);

//This constructor is /only/ called by the loader
AidsActionDeployMech::AidsActionDeployMech() : AidsAction(vec3(),0) {
	//Loader runs directly after this
}


//Constructing the action should start it
AidsActionDeployMech::AidsActionDeployMech(vec3 actionLocation) : AidsAction(actionLocation,GetIntensityValue()) {
	//Find a hidden location
	vec3 deplyLocation = findHiddenLocation(actionLocation,3);
	//Deploy
	deployed = Game()->Actors.BuildActor<ActorAIMech>(deplyLocation);
}

//Every subclass should have a static member GetIntensityValue()
float AidsActionDeployMech::GetIntensityValue() {
	return 35.0f;
}


//If this returns true clean up the action
//the action has ended
bool AidsActionDeployMech::Update() {
	//If they're dead, they're dead
	if (deployed->Dead())
		return true;
	//If they're too far away, undeploy them
	if (glm::distance(deployed->GetPosition(),Game()->Actors.Player()->GetPosition()) > 300) {
		deployed->Kill();
	}
	return false;
}