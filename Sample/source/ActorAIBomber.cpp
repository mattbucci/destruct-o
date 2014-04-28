#include "stdafx.h"
#include "ActorAIBomber.h"
#include "ModelInstance.h"
#include "BombDropAIWeapon.h"
#include "BaseFrame.h"

CLASS_SAVE_CONSTRUCTOR(ActorAIBomber);

ActorAIBomber::ActorAIBomber() : ActorAI(new BombDropAIWeapon(this),20,vec3(5,5,2))  {
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
	if (glm::length(vec2(targetEnemy->GetPosition()) - vec2(this->GetPosition())) < sightDistance()) {
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
			vec2 moveVelocity = glm::normalize(diff)*baseMovementSpeed();
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
			Velocity = vec3(vec2(cos(this->facingDirection), sin(this->facingDirection))*baseMovementSpeed(), Velocity.z);

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


//Attempts to snap the user's spine to face the enemy
//returns true if successful
//Check if your spine can face the enemy right now
bool ActorAIBomber::checkSpineLimits() {
	//Since you can move vertically pretty well
	//just check if the horizontal is very close to 0
	vec2 diff = vec2(targetEnemy->GetPosition()) - vec2(Position);
			
	float desired = atan2f(diff.y,diff.x);
	float horizontalDiff = abs(fmodf(desired - facingDirection,(float)M_PI));
	//Check against a tight angle check
	return horizontalDiff < M_PI/90.0f;
}

//Snap the model's skeleton to face the enemy
void ActorAIBomber::snapSpineToEnemy() {
	//Right now don't even try
	return;

	
}

void ActorAIBomber::findMuzzlePosition() {
	//Find bone
	/*Node::const_flattreemap flatmap;
	model->Skeleton()->GetFlatNodeTree(flatmap);
	const Node * nA = model->Skeleton()->FindNode("rt_shoulder");
	mat4 globalTransformA = model->GetTransform().TransformMatrix() * nA->TransformMatrix();

	const Node * nB = model->Skeleton()->FindNode("lf_shoulder");
	mat4 globalTransformB = model->GetTransform().TransformMatrix() * nB->TransformMatrix();

		
	//These look mediocre at best
	//if someone wants to fix them that would be awesome
	vec3 aVector(.2,-.25,-.8);
	vec3 bVector(-.2,-.3,1.6);
	//Calculate the position along the muzzle
	muzzlePositionA = vec3(globalTransformA * vec4(aVector,1.0));
	muzzlePositionB = vec3(globalTransformB * vec4(bVector,1.0));*/

	//Get 90 degree difference from the facing vector
	vec2 fireDir = glm::normalize(vec2(cos(facingDirection+M_PI/2.0f),sin(facingDirection+M_PI/2.0f)));

	muzzlePositionA = Position+vec3(fireDir*2.0f,0.0f);
	muzzlePositionB = Position-vec3(fireDir*2.0f,0.0f);
}


//DEFAULTS:
//The time it takes to target after finding the enemy
double ActorAIBomber::targetTime() {
	return 3.0;
}

//The movement speed of this enemy
//should be tuned to walk animation speed
float ActorAIBomber::baseMovementSpeed() {
	return 50;
}

//How far can enemies spot each other
float ActorAIBomber::sightDistance() {
	return 30;
}

//How many radians per second this actor can rotate
float ActorAIBomber::turnSpeed() {
	return (float)(M_PI/2.5);
}

//The altitude to hover at normally
float ActorAIBomber::flyHeight() {
	return 100;
}

//The fastest this aircraft can correct its altitude
float ActorAIBomber::altitudeChangeRate() {
	return 5;
}