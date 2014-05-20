
#pragma once

#include "PhysicsActor.h"

//Anything fired by the EntityFireWeapon must be a projectile
class ActorProjectile : public PhysicsActor {
protected:
	PhysicsActor * firingActor;
	vec3 spawnPos;
public:
	ActorProjectile();

	virtual ~ActorProjectile();

	//Fire this projectile
	virtual void FireProjectile(PhysicsActor * firingActor, vec3 fromPosition, vec3 velocityVector);

	virtual bool Update() override;

	CLASS_DECLARATION(ActorProjectile)
		INHERITS_FROM(PhysicsActor)
		CLASS_MEMBER(firingActor,ReflectionData::SAVE_OWNEDHANDLE);
	END_DECLARATION
};