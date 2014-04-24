#include "stdafx.h"
#include "ActorAIBomber.h"
#include "ModelInstance.h"
#include "BombDropAIWeapon.h"

CLASS_SAVE_CONSTRUCTOR(ActorAIBomber);

ActorAIBomber::ActorAIBomber() : ActorAI(new BombDropAIWeapon(this),20,vec3(5,5,2))  {
	flying = true;
	scale = 3;
	runStarted = false;
	runFinished = false;
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
	//Move towards your run direction
	Velocity = vec3(runDirection*baseMovementSpeed(),Velocity.z);

	//Check if the run is done
	if (glm::distance(runStartPosition,vec2(Position)) > sightDistance()*2.0f)
		runFinished = true;

	//Face the direction your run is moving
	facingDirection = atan2(runDirection.y,runDirection.x);

	//During your run, bomb the hell out of the enemy
	if (!runFinished) {
		weapon->Update(vec3(),muzzlePositionA,muzzlePositionB);
		weapon->HoldingTrigger(true);
		holdingTrigger = true;
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
	float horizontalDiff = abs(fmodf(desired - facingDirection,M_PI));
	//Check against a tight angle check
	return horizontalDiff < M_PI/90.0f;
}

//Snap the model's skeleton to face the enemy
void ActorAIBomber::snapSpineToEnemy() {
	//Right now don't even try
	return;

	
}

//Whether or not a bombing run completed
bool ActorAIBomber::BombingRunComplete() {
	return runFinished;
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
	return 15;
}

//How far can enemies spot each other
float ActorAIBomber::sightDistance() {
	return 45;
}

//How many radians per second this actor can rotate
float ActorAIBomber::turnSpeed() {
	return (float)(M_PI/2.5);
}

//The altitude to hover at normally
float ActorAIBomber::flyHeight() {
	return 20;
}

//The fastest this aircraft can correct its altitude
float ActorAIBomber::altitudeChangeRate() {
	return 5;
}