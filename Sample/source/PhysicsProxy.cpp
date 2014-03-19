#include "stdafx.h"
#include "PhysicsProxy.h"


PhysicsProxy::PhysicsProxy() {

}
PhysicsProxy::~PhysicsProxy() {

}

//Update Position/Velocity
//and zero out the acceleration afterwards
//apply changes to the actor this proxy is connected to
void PhysicsProxy::Apply(float delta) {

}

//Check for the special case of a collision with an axis aligned voxel
bool PhysicsProxy::AABBCollision(vec3 voxelPosition) {
	
}
//Check for a collision with another physicsproxy
bool PhysicsProxy::AAABBCollision(PhysicsProxy * other) {

}