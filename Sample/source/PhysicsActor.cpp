#include "stdafx.h"
#include "PhysicsActor.h"
#include "BaseFrame.h"

PhysicsActor::PhysicsActor(vec3 size) :
	Velocity(velocity),
	Acceleration(acceleration),
	Position(position) {
	this->size = size;
	onGround = false;
	Game()->Physics.RegisterPhysicsActor(this);
}
PhysicsActor::~PhysicsActor() {
	Game()->Physics.UnregisterPhysicsActor(this);
}

//Get the position of this physics actor
vec3 PhysicsActor::GetPosition() {
	return position;
}

bool PhysicsActor::OnGround() {
	return onGround;
}

//Apply velocity/acceleration to this actor
bool PhysicsActor::Update() {
	return Actor::Update();
}
//Check for the special case of a collision with an axis aligned voxel
//The fed position should be the center of the voxel
bool PhysicsActor::aabbCollision(vec3 voxelPosition) {
	vec3 voxelStart = voxelPosition-.5f;
	vec3 voxelEnd = voxelPosition+.5f;
	vec3 myPos = position - size*.5f;
	vec3 myEnd = position + size*.5f;
	return !((myPos.x > voxelEnd.x) ||
			(myEnd.x < voxelStart.x) ||
			(myPos.y > voxelEnd.y) ||
			(myEnd.y < voxelStart.y) ||
			(myPos.z > voxelEnd.z) ||
			(myEnd.z < voxelStart.z));
}
//Check for a collision with another physicsproxy
bool PhysicsActor::aabbCollision(PhysicsActor * other) {
	vec3 voxelStart = other->position - other->size*.5f;
	vec3 voxelEnd = other->position + other->size*.5f;
	vec3 myPos = position - size*.5f;
	vec3 myEnd = position + size*.5f;
	return !((myPos.x > voxelEnd.x) ||
			(myEnd.x < voxelStart.x) ||
			(myPos.y > voxelEnd.y) ||
			(myEnd.y < voxelStart.y) ||
			(myPos.z > voxelEnd.z) ||
			(myEnd.z < voxelStart.z));
}

