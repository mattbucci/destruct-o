#include "stdafx.h"
#include "AidsAction.h"
#include "Universal.h"
#include "BaseFrame.h"

vec3 AidsAction::findHiddenLocation(vec3 startLocation, float desiredAltitude) {
	const static float anglePart = M_PI/4.0f;
	//Only tries 10 locations
	//don't want to do too many ray traces 
	const static float distancePart = 5;
	float distanceOffset = Utilities::random(20,30);
	float angleOffset = Utilities::random(-M_PI,M_PI);

	vec3 lastPos;
	ActorPlayer * player = Game()->Actors.Player();

	for (int i = 0; i < 10; i++) {
		float angle = i*anglePart;
		float distance = pow((float)i,1.3f)*distancePart+distanceOffset;
		//Calculate the guess
		vec2 twoDGuess = vec2(cos(angle+angleOffset)*distance,sin(angle+angleOffset)*distance);
		lastPos = vec3(twoDGuess,Game()->Voxels.GetPositionHeight(twoDGuess)+desiredAltitude);
		
		//Check if you're obscured by fog
		if (glm::distance(lastPos,player->GetPosition()) > Game()->ViewDistance.GetViewDistance())
			//This point is in the fog
			break;

		PhysicsActor * actorHit;
		if (!Universal::Trace(lastPos,glm::normalize(player->GetPosition() - lastPos),NULL,&actorHit)) {
			//Failed to find the actor
			if (actorHit == player)
				continue;
		}

		//This point is good
		break;
	}
	return lastPos;
}


//Constructing the action should start it
AidsAction::AidsAction(vec3 actionLocation, float actionCost) {
	this->actionCost = actionCost;
	this->actionLocation = actionLocation;
}
AidsAction::~AidsAction() {

}

//Every subclass should have a static member GetIntensityValue()

//retrieve the cost paid for doing this action
float AidsAction::GetIntensityCost() {
	return actionCost;
}