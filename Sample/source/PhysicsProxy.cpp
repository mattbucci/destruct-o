#include "stdafx.h"
#include "PhysicsProxy.h"
#include "Actor.h"


PhysicsProxy::PhysicsProxy(Actor * effector, vec3 * effectorPosition) {
	this->effector = effector;
	this->effectorPosition = effectorPosition;
	Position = *effectorPosition;
}
PhysicsProxy::~PhysicsProxy() {

}

//Update Position/Velocity
//and zero out the acceleration afterwards
//apply changes to the actor this proxy is connected to
bool PhysicsProxy::Apply(float delta) {
	//this proxy is no longer valid
	if (effector == NULL)
		return false;

	//How much has the player moved due to actor interaction
	vec3 playerDelta = Position - *effectorPosition;
	//Apply forces and clear them for the next calculation
	//add the player's movement in as velocity
	Velocity += Acceleration*delta + (playerDelta*delta);
	Position += Velocity*delta;
	Acceleration = vec3();
	//Apply the position back to the effector
	*effectorPosition = Position;

	//Indicate that this proxy is still valid
	return true;
}

//Check for the special case of a collision with an axis aligned voxel
bool PhysicsProxy::AABBCollision(vec3 voxelPosition) {
	vec3 end = voxelPosition+1.0f;
	vec3 myEnd = Position + size;
	return !((Position.x > end.x) ||
			(myEnd.x < voxelPosition.x) ||
			(Position.y > end.y) ||
			(myEnd.y < voxelPosition.y) ||
			(Position.z > end.z) ||
			(myEnd.z < voxelPosition.z));
}
//Check for a collision with another physicsproxy
bool PhysicsProxy::AABBCollision(PhysicsProxy * other) {
	vec3 voxelPosition = other->Position;
	vec3 end = voxelPosition+other->size;
	vec3 myEnd = Position + size;
	return !((Position.x > end.x) ||
			(myEnd.x < voxelPosition.x) ||
			(Position.y > end.y) ||
			(myEnd.y < voxelPosition.y) ||
			(Position.z > end.z) ||
			(myEnd.z < voxelPosition.z));
}

void PhysicsProxy::DetachFromEffector() {
	effector = NULL;
}