#include "stdafx.h"
#include "ActorAIBomber.h"
#include "ModelInstance.h"
#include "BaseFrame.h"
#include "WeaponAI.h"

CLASS_SAVE_CONSTRUCTOR(ActorAIBomber);

ActorAIBomber::ActorAIBomber() : ActorAI()  {
	flying = true;
	scale = 2;
	runStarted = false;
	turnarounddistance = 150;
}
ActorAIBomber::~ActorAIBomber() {

}

//Path until you're in range of the bomb run
void ActorAIBomber::stateEngaging(bool & holdingTrigger) {
	if (!runStarted) {
		//Plan the bombing run
		//check you've got an enemy
		if (targetEnemy == NULL) {
			state = AI_SCANNING;
			return;
		}
		//Set the run goal
		vec2 goal = vec2(targetEnemy->GetPosition());
		//Pick a point past that
		runDirection = glm::normalize(goal - vec2(Position));
		runStartPosition = vec2(Position);
		runStarted = true;
	}

	//During your run, bomb the hell out of the enemy
	if (glm::length(vec2(targetEnemy->GetPosition()) - vec2(this->GetPosition())) < data->SightDistance) {
		weapon->Update(vec3(),muzzlePositionA,muzzlePositionB);
		weapon->HoldingTrigger(true);
		holdingTrigger = true;
	}
}

void ActorAIBomber::cheapUpdate() {
	//AI is a state machine
	switch (state) {
	case AI_WAITINGFORPATH:
	case AI_PATHING:
		{
			//Face the direction you're moving
			vec2 diff = goal - vec2(Position);
			
			applyFacingDirection(atan2f(diff.y,diff.x));
			//Apply some velocity in the direction you want to move
			vec2 moveVelocity = glm::normalize(diff)*data->BaseMovementSpeed;
			Velocity = vec3(moveVelocity,Velocity.z);
		}
		break;
	case AI_SCANNING:
		break;
	case AI_TARGETING_ENEMY:
		//If you have an enemy, face them
		if (targetEnemy != NULL) 
			//Face the enemy you're targeting
			faceEnemy();
		break;
	case AI_ENGAGING_ENEMY:
		{
			//Move towards your forward direction
			Velocity = vec3(vec2(cos(this->facingDirection), sin(this->facingDirection))*data->BaseMovementSpeed, Velocity.z);

			if (glm::length(vec2(targetEnemy->GetPosition()) - vec2(this->GetPosition())) > turnarounddistance) {
				runStarted = false;
			}

			//Face the direction your run is moving
			this->applyFacingDirection(atan2(runDirection.y,runDirection.x));
		}
		break;
	case AI_DYING:
		break;
	case AI_ROTTING:
		break;
	}
}

