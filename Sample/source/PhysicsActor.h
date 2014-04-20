
#pragma once

#include "Actor.h"

#include "GameFactions.h"


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
	double lastDamageRecieved;

	//The physics system has direct access to position/velocity/acceleration
	friend class PhysicsSystem;
protected:
	//Get/Set the properties of this physics actor
	vec3 & Velocity;
	vec3 & Position;
	vec3 & Acceleration;
	const vec3 & Size;
	//Status checkups
	bool OnGround();
	//if you've taken any damage in the last 4 seconds this is true
	bool BeingDamaged();

	//The max health you can have
	float maxLife;

	//your life (starts at max)
	float life;

	//The faction this actor is on
	FactionId faction;

	//Whether or not this actor can take damage
	//invulnerable things cannot experience death
	bool vulnerable;

	//Override to prevent immediate death
	virtual void onDeath();
public:
	PhysicsActor(vec3 size, float maxLife, FactionId faction);
	~PhysicsActor();

	//Anyone can know the position
	vec3 GetPosition();

	//Get the current life of this actor
	float GetLife();

	//Get the faction of this actor
	FactionId GetFaction();

	//Damage this actor from a particular faction
	virtual void Damage(FactionId damagingFaction, float damage);

	//Damage this actor from a particular actor
	virtual void Damage(PhysicsActor * damagingActor, float damage);

	//Get the max life of this actor
	virtual float GetMaxLife();

	//Draw this actor
	virtual void Draw(MaterialProgram * materialShader) override;

	CLASS_DECLARATION(PhysicsActor)
		INHERITS_FROM(Actor)
		CLASS_MEMBER(position,ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(velocity,ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(acceleration,ReflectionData::SAVE_VEC3)
	END_DECLARATION
};