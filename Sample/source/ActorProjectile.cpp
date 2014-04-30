#include "stdafx.h"
#include "ActorProjectile.h"

ActorProjectile::ActorProjectile(FactionId faction) : PhysicsActor(faction) {

}

ActorProjectile::~ActorProjectile() {

}

void ActorProjectile::FireProjectile(vec3 fromPosition, vec3 velocityVector) {
	Position = fromPosition;
	Velocity = velocityVector;
	//set your facing direction to match the velocity vector
	facingDirection = atan2(Velocity.y,Velocity.x);
	//TODO: handle 3d case
}