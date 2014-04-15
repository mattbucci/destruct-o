#include "stdafx.h"
#include "PhysicsActor.h"

PhysicsActor::PhysicsActor() {

}
PhysicsActor::~PhysicsActor() {

}

//Get the position of this physics actor
vec3 PhysicsActor::GetPosition() {
	return Position;
}

//Apply velocity/acceleration to this actor
bool PhysicsActor::Update(float delta, float now) {
	return Actor::Update(delta,now);
}


	//Check for the special case of a collision with an axis aligned voxel
//The fed position should be the center of the voxel
bool PhysicsActor::aabbCollision(vec3 voxelPosition) {
	vec3 end = voxelPosition+1.0f;
	vec3 myPos = Position - size*.5f;
	vec3 myEnd = Position + size*.5f;
	return !((myPos.x > end.x) ||
			(myEnd.x < voxelPosition.x) ||
			(myPos.y > end.y) ||
			(myEnd.y < voxelPosition.y) ||
			(myPos.z > end.z) ||
			(myEnd.z < voxelPosition.z));
}
//Check for a collision with another physicsproxy
bool PhysicsActor::aabbCollision(PhysicsActor * other) {
	vec3 voxelPosition = other->Position;
	vec3 end = voxelPosition+other->size*.5f;
	vec3 position = voxelPosition-other->size*.5f;
	vec3 myPos = position - size*.5f;
	vec3 myEnd = position + size*.5f;
	return !((myPos.x > end.x) ||
			(myEnd.x < position.x) ||
			(myPos.y > end.y) ||
			(myEnd.y < position.y) ||
			(myPos.z > end.z) ||
			(myEnd.z < position.z));
}

