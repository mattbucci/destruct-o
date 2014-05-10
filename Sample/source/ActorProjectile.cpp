#include "stdafx.h"
#include "ActorProjectile.h"
#include "BaseFrame.h"

ActorProjectile::ActorProjectile() : PhysicsActor(GameFactions::FACTION_HOSTILE) {
	firingActor = NULL;
}

ActorProjectile::~ActorProjectile() {

}

void ActorProjectile::FireProjectile(PhysicsActor * firingActor, vec3 fromPosition, vec3 velocityVector) {
	this->SetFaction(firingActor->GetFaction());
	this->firingActor = firingActor;

	Position = fromPosition;
	Velocity = velocityVector;
	//set your facing direction to match the velocity vector
	facingDirection = atan2(Velocity.y,Velocity.x);
	//TODO: handle 3d case
	spawnPos = fromPosition;
}

bool ActorProjectile::Update() {
	//projectiles have fixed lifespans
	if (glm::distance(spawnPos,Position) > 150)
		Destroy();


	return PhysicsActor::Update();
}