#include "stdafx.h"
#include "AidsActionDeploySingle.h"
#include "BaseFrame.h"

#include "ActorPlayer.h"
#include "ActorAids.h"
#include "ActorAI.h"

CLASS_SAVE_CONSTRUCTOR(AidsActionDeploySingle);

//This constructor is /only/ called by the loader
AidsActionDeploySingle::AidsActionDeploySingle() : AidsAction(vec3(),0) {
	//Loader runs directly after this
}


//Constructing the action should start it
AidsActionDeploySingle::AidsActionDeploySingle(vec3 actionLocation, float actionCost, string actorToDeploy) : AidsAction(actionLocation, actionCost) {
	//Find a hidden location
	vec3 deplyLocation = findHiddenLocation(actionLocation,3);
	//Deploy
	deployed = Game()->Actors.BuildAI(deplyLocation,actorToDeploy);
	//Calculate the appropriate faction for this AI which is being deployed
	deployed->SetFaction(Game()->Actors.Aids()->FindAIFactionOfPoint(vec2(deplyLocation)));
}

//If this returns true clean up the action
//the action has ended
bool AidsActionDeploySingle::Update() {
	//If they're dead, they're dead
	if (deployed->Dead())
		return true;
	//If they're too far away, undeploy them
	if (glm::distance(deployed->GetPosition(),Game()->Actors.Player()->GetPosition()) > 300) {
		deployed->Kill();
	}
	return false;
}