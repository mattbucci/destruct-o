#include "stdafx.h"
#include "ActorAISoldier.h"
#include "ModelInstance.h"
#include "SoldierAIWeapon.h"

ActorAISoldier::ActorAISoldier() : ActorAI(new SoldierAIWeapon(this,energyPool))  {

}
ActorAISoldier::~ActorAISoldier() {

}

//Attempts to snap the user's spine to face the enemy
//returns true if successful
//Check if your spine can face the enemy right now
bool ActorAISoldier::checkSpineLimits() {
	//Since you can move vertically pretty well
	//just check if the horizontal is very close to 0
	vec2 diff = vec2(targetEnemy->GetPosition()) - vec2(Position);
			
	float desired = atan2f(diff.y,diff.x);
	float horizontalDiff = abs(fmodf(desired - facingDirection,M_PI));
	//Check against a tight angle check
	return horizontalDiff < M_PI/90.0f;
}

//Snap the model's skeleton to face the enemy
void ActorAISoldier::snapSpineToEnemy() {
	//Take a guess at the correction in 2d only
	float distance = glm::length(targetEnemy->GetPosition() - Position);
	vec3 correctVector = glm::normalize(vec3(0,distance,targetEnemy->GetPosition().z - 1.5f - Position.z));
	//Get angle between them
	float angle = atan2(-correctVector.z,correctVector.y);
	

	vec3 axis = vec3(1,0,0);//glm::normalize(glm::cross(desiredFace,facing));
	//float angle = fmod(OS::Now()/2,M_PI) - .5f*M_PI;
	
	quat rotation = glm::quat(glm::rotate(angle,axis));
	//Apply to the spine
	Node * spineNode = model->Animation().Skeleton()->FindNode("Bind_Spine1");


	spineNode->LocalTransform().Rotation() = glm::rotate(spineNode->LocalTransform().Rotation(),angle/M_PI*180.0f,axis);

	spineNode->Recalculate();
}

void ActorAISoldier::findMuzzlePosition() {
	//Find bone
	Node::const_flattreemap flatmap;
	model->Skeleton()->GetFlatNodeTree(flatmap);
	const Node * n = model->Skeleton()->FindNode("Bind_RightHand");
	mat4 globalTransform = model->GetTransform().TransformMatrix() * n->TransformMatrix();

		

	vec3 mVector(0,.75,.25);
	//Calculate the position along the muzzle
	muzzlePositionA =  vec3(globalTransform * vec4(mVector,1.0));
}


//DEFAULTS:
//The time it takes to target after finding the enemy
double ActorAISoldier::targetTime() {
	return 3.0;
}

//The movement speed of this enemy
//should be tuned to walk animation speed
float ActorAISoldier::baseMovementSpeed() {
	return 6;
}

//How far can enemies spot each other
float ActorAISoldier::sightDistance() {
	return 30;
}

//How many radians per second this actor can rotate
float ActorAISoldier::turnSpeed() {
	return (float)(M_PI/2.5);
}
