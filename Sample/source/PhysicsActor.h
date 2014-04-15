
#pragma once

#include "Actor.h"


class PhysicsActor : public Actor {
	//Size is the width/length/height of the hitbox
	vec3 size;

	//Check for the special case of a collision with an axis aligned voxel
	//The fed position should be the center of the voxel
	bool aabbCollision(vec3 voxelPosition);
	//Check for a collision with another physicsproxy
	bool aabbCollision(PhysicsActor * other);

	//The physics system has direct access to position/velocity/acceleration
	friend class PhysicsSystem;

public:
	PhysicsActor();
	~PhysicsActor();

	//Get the position of this physics actor
	vec3 GetPosition();

	//Apply velocity/acceleration to this actor
	virtual bool Update(float delta, float now);

	//Actors should not use these methods to enact changes (except maybe position)
	//position of the physics proxy is the center
	vec3 Position;
	vec3 Velocity;
	vec3 Acceleration;

	CLASS_DECLARATION(PhysicsActor)
		INHERITS_FROM(Actor)
		CLASS_MEMBER(Position,ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(Velocity,ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(Acceleration,ReflectionData::SAVE_VEC3)
	END_DECLARATION
};