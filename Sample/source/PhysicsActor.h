
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
	vec3 velocity;
	vec3 acceleration;

	//If the actor has experienced an upwards force from solid ground this is true
	bool onGround;
	double lastDamageRecieved;

	//if the physics actor is colliding with another physics actor this is the actor
	PhysicsActor * collidingWith;
	//If the physics actor is colliding with anything (the ground or another physics actor) this is true
	bool colliding;

	//The physics system has direct access to position/velocity/acceleration
	friend class PhysicsSystem;
	//This is a little hacky, but so does the ActorSystem
	friend class ActorSystem;
protected:
	//Get/Set the properties of this physics actor
	vec3 & Velocity;
	vec3 & Position;
	vec3 & Acceleration;
	vec3 & Size;
	//Whether or not this actor flies
	//(doesn't experience gravity)
	bool flying;
	//Status checkups
	bool OnGround();
	//if you've taken any damage in the last 4 seconds this is true
	bool BeingDamaged();
	//If you're touching anything, this is true
	bool Colliding();
	//If what you're touching is an actor, this is that actor
	PhysicsActor * CollidingWith();

	//The rate at which you regain health
	float lifeRegenRate;

	//The max health you can have
	float maxLife;

	//your life (starts at max)
	float life;

	//the rate at which you regain energy
	float energyRegenRate;

	//the max energy in this units energy pool
	float energyPoolMax;

	//The energy pool for firing a weapon
	float energyPool;

	//The faction this actor is on
	FactionId faction;

	//Whether or not this actor can take damage
	//invulnerable things cannot experience death
	bool vulnerable;

	//Override to prevent immediate death
	virtual void onDeath();
public:
	PhysicsActor(FactionId faction);
	virtual ~PhysicsActor();

	//Get the current life of this actor
	float GetLife();

	//Get the percentage of life this actor has left
	//0 = 0%, 1 = 100%
	float GetLifePercentage();

	//Get the faction of this actor
	FactionId GetFaction();

	//Damage this actor from a particular faction
	virtual void Damage(FactionId damagingFaction, float damage);

	//Damage this actor from a particular actor
	//this should be preferred, always use this when an actor
	//is available
	virtual void Damage(PhysicsActor * damagingActor, float damage);

	//Get the max life of this actor
	virtual float GetMaxLife();

	//Draw this actor
	virtual void Draw(MaterialProgram * materialShader) override;

	//Access to the energy pool for weapons
	//kind of hacky
	float & EnergyPool();

	//Change the faction of this actor
	void SetFaction(FactionId faction);

	CLASS_DECLARATION(PhysicsActor)
		INHERITS_FROM(Actor)
		CLASS_MEMBER(velocity,ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(acceleration,ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(size,ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(onGround,ReflectionData::SAVE_BOOL)

		CLASS_MEMBER(flying,ReflectionData::SAVE_BOOL)
		CLASS_MEMBER(energyPool,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(maxLife,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(life,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(lastDamageRecieved,ReflectionData::SAVE_DOUBLE)

		CLASS_MEMBER(faction,ReflectionData::SAVE_INT32)
		CLASS_MEMBER(colliding,ReflectionData::SAVE_BOOL)
		CLASS_MEMBER(collidingWith,ReflectionData::SAVE_HANDLE)
	END_DECLARATION
};