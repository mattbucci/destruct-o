
#pragma once

#include "stdafx.h"

class Actor;

class PhysicsProxy {
	//The actor this physics proxy is connected to
	Actor * effector;
	vec3 * effectorPosition;
	vec3 size;
public:
	PhysicsProxy(Actor * effector, vec3 * effectorPosition);
	~PhysicsProxy();

	//The full extents
	vec3 Extents;

	//Update Position/Velocity
	//and zero out the acceleration afterwards
	//apply changes to the actor this proxy is connected to
	//returns true if this object should be destroyed
	bool Apply(float delta);

	//Check for the special case of a collision with an axis aligned voxel
	//The fed position should be the center of the voxel
	bool AABBCollision(vec3 voxelPosition);
	//Check for a collision with another physicsproxy
	bool AABBCollision(PhysicsProxy * other);

	//Schedule this proxy for destruction detaching the actor automatically
	void DetachFromEffector();

	//Actors should not use these methods to enact changes (except maybe position)
	//position of the physics proxy is the lower left
	vec3 Position;
	vec3 Velocity;
	vec3 Acceleration;
};