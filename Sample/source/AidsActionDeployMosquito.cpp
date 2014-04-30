#include "stdafx.h"
#include "AidsActionDeployMosquito.h"
#include "BaseFrame.h"

#include "ActorAI.h"

CLASS_SAVE_CONSTRUCTOR(AidsActionDeployMosquito);

//This constructor is /only/ called by the loader
AidsActionDeployMosquito::AidsActionDeployMosquito() : AidsAction(vec3(),0) {
	//Loader runs directly after this
}


//Constructing the action should start it
AidsActionDeployMosquito::AidsActionDeployMosquito(vec3 actionLocation) : AidsAction(actionLocation,GetIntensityValue()) {
	//Find a hidden location
	vec3 deplyLocation = findHiddenLocation(actionLocation,3);
	//Deploy
	deployed = Game()->Actors.BuildAI(deplyLocation,"helicopter.json");
}

//Every subclass should have a static member GetIntensityValue()
float AidsActionDeployMosquito::GetIntensityValue() {
	return 50.0f;
}


//If this returns true clean up the action
//the action has ended
bool AidsActionDeployMosquito::Update() {
	//If they're dead, they're dead
	if (deployed->Dead())
		return true;
	//If they're too far away, undeploy them
	if (glm::distance(deployed->GetPosition(),Game()->Actors.Player()->GetPosition()) > 300) {
		deployed->Kill();
	}
	return false;
}