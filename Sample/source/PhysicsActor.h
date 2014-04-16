
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

	//Actors should not use these methods to enact changes (except maybe position)
	//position of the physics proxy is the center
	vec3 position;
	vec3 velocity;
	vec3 acceleration;

	bool onGround;

	//The physics system has direct access to position/velocity/acceleration
	friend class PhysicsSystem;
protected:
	//Get/Set the properties of this physics actor
	vec3 & Velocity;
	vec3 & Position;
	vec3 & Acceleration;
	bool OnGround();
public:
	PhysicsActor(vec3 size);
	~PhysicsActor();

	//Anyone can know the position
	vec3 GetPosition();


	//Apply velocity/acceleration to this actor
	virtual bool Update(float delta, float now);



	CLASS_DECLARATION(PhysicsActor)
		INHERITS_FROM(Actor)
		CLASS_MEMBER(position,ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(velocity,ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(acceleration,ReflectionData::SAVE_VEC3)
	END_DECLARATION
};