
#pragma once

#include "PhysicsActor.h"

//Anything fired by the EntityFireWeapon must be a projectile
class ActorProjectile : public PhysicsActor {
public:
	ActorProjectile(FactionId faction);

	virtual ~ActorProjectile();

	void FireProjectile(vec3 fromPosition, vec3 velocityVector);
};