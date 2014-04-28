#include "stdafx.h"
#include "ActorAITurret.h"
#include "ModelInstance.h"
#include "TurretAIWeapon.h"

CLASS_SAVE_CONSTRUCTOR(ActorAITurret);

ActorAITurret::ActorAITurret() : ActorAI(new TurretAIWeapon(this),100,vec3(3,3,2.5))  {

}
ActorAITurret::~ActorAITurret() {

}

//Attempts to snap the user's spine to face the enemy
//returns true if successful
//Check if your spine can face the enemy right now
bool ActorAITurret::checkSpineLimits() {
	//Since you can move vertically pretty well
	//just check if the horizontal is very close to 0
	vec2 diff = vec2(targetEnemy->GetPosition()) - vec2(Position);
			
	float desired = atan2f(diff.y,diff.x);
	float horizontalDiff = abs(fmodf(desired - facingDirection,M_PI));
	//Check against a tight angle check
	return horizontalDiff < M_PI/90.0f;
}

//Snap the model's skeleton to face the enemy
void ActorAITurret::snapSpineToEnemy() {
	//Take a guess at the correction in 2d only
	float distance = glm::length(targetEnemy->GetPosition() - Position);
	vec3 correctVector = glm::normalize(vec3(0,distance,targetEnemy->GetPosition().z - 1.5f - Position.z));
	//Get angle between them
	float angle = atan2(-correctVector.z,correctVector.y);
	

	vec3 axis = vec3(1,0,0);//glm::normalize(glm::cross(desiredFace,facing));
	//float angle = fmod(OS::Now()/2,M_PI) - .5f*M_PI;
	//quat rotation = glm::quat(glm::rotate(angle,axis));
    
	//Apply to the spine
	Node * spineNode = model->Animation().Skeleton()->FindNode("Hig_Poly_Guns");
	angle -= M_PI/2.0;
	spineNode->LocalTransform().Rotation() = glm::rotate(quat(),angle/M_PI*180.0f,axis);
	spineNode->Recalculate();
}

void ActorAITurret::findMuzzlePosition() {
	Node::const_flattreemap flatMap;
	model->Skeleton()->GetFlatNodeTree(flatMap);
	//Find bone
	const Node * n = model->Skeleton()->FindNode("Hig_Poly_Guns");
	mat4 globalTransform = model->GetTransform().TransformMatrix() * n->TransformMatrix();
	vec3 aVector(1.3,-2.4,0);
	vec3 bVector(-1.3,-2.4,0);
	//Calculate the position along the muzzle
	muzzlePositionA =  vec3(globalTransform * vec4(aVector,1.0));
	muzzlePositionB =  vec3(globalTransform * vec4(bVector,1.0));
}


//DEFAULTS:
//The time it takes to target after finding the enemy
double ActorAITurret::targetTime() {
	return 3.0;
}

//The movement speed of this enemy
//should be tuned to walk animation speed
float ActorAITurret::baseMovementSpeed() {
	return 0;
}

//How far can enemies spot each other
float ActorAITurret::sightDistance() {
	return 30;
}

//How many radians per second this actor can rotate
float ActorAITurret::turnSpeed() {
	return (float)(M_PI/2.5);
}
