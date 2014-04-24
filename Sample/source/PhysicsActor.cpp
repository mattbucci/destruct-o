#include "stdafx.h"
#include "PhysicsActor.h"
#include "BaseFrame.h"

PhysicsActor::PhysicsActor(vec3 size, float maxLife, FactionId faction) :
	Velocity(velocity),
	Acceleration(acceleration),
	Position(position),
	Size(this->size){

	//Save settings
	this->size = size;
	this->maxLife = maxLife;
	this->faction = faction;
	//Apply defaults
	onGround = false;
	lastDamageRecieved = 0;
	life = maxLife;
	vulnerable = true;
	faction = GameFactions::FACTION_HOSTILE;
	flying = false;
}
PhysicsActor::~PhysicsActor() {
	
}


//Access to the energy pool for weapons
//kind of hacky
float & PhysicsActor::EnergyPool() {
	return energyPool;
}

//Override to prevent immediate death
void PhysicsActor::onDeath() {
	//On death destroy this actor
	Destroy();
}

bool PhysicsActor::OnGround() {
	return onGround;
}

bool PhysicsActor::BeingDamaged() {
	return Game()->Now() < lastDamageRecieved+5;
}

//Get the current life of this actor
float PhysicsActor::GetLife() {
	return life;
}

//Damage this actor from a particular faction
void PhysicsActor::Damage(FactionId damagingFaction, float damage) {
	//Can't take damage if you're invulnerable
	if (!vulnerable)
		return;
	//Check that you're alive
	if (life > 0) {
		lastDamageRecieved = Game()->Now();
		//Fire the damaged event
		Game()->Actors.ActorDamaged.Fire([this,damagingFaction,damage](function<void(Actor*,Actor*,FactionId,float)> observer) {
			observer(this,NULL,damagingFaction,damage);
		});
		//Apply damage
		life -= damage;
		if (life < 0)
			life = 0;
		if (life <= 0) {
			//Fire on death event
			Game()->Actors.ActorKilled.Fire([this,damagingFaction](function<void(Actor*,Actor*,FactionId)> observer) {
				observer(this,NULL,damagingFaction);
			});
			//This actor just died
			onDeath();
		}
	}
}

//Damage this actor from a particular actor
void PhysicsActor::Damage(PhysicsActor * damagingActor, float damage) {
	//Can't take damage if you're invulnerable
	if (!vulnerable)
		return;
	//Check that you're alive
	if (life > 0) {
		lastDamageRecieved = Game()->Now();
		//Fire the damaged event
		Game()->Actors.ActorDamaged.Fire([this,damagingActor,damage](function<void(Actor*,Actor*,FactionId,float)> observer) {
			observer(this,damagingActor,damagingActor->faction,damage);
		});
		//Apply damage
		life -= damage;
		if (life < 0)
			life = 0;
		if (life <= 0) {
			//Fire on death event
			Game()->Actors.ActorKilled.Fire([this,damagingActor](function<void(Actor*,Actor*,FactionId)> observer) {
				observer(this,damagingActor,damagingActor->faction);
			});
			//This actor just died
			onDeath();
		}
	}
}

//Get the max life of this actor
float PhysicsActor::GetMaxLife() {
	return maxLife;
}

//Get the faction of this actor
FactionId PhysicsActor::GetFaction() {
	return faction;
}


//Check for the special case of a collision with an axis aligned voxel
//The fed position should be the center of the voxel
bool PhysicsActor::aabbCollision(vec3 voxelPosition) {
	vec3 voxelStart = voxelPosition-.5f;
	vec3 voxelEnd = voxelPosition+.5f;
	vec3 myPos = position - size*.5f;
	vec3 myEnd = position + size*.5f;
	return !((myPos.x > voxelEnd.x) ||
			(myEnd.x < voxelStart.x) ||
			(myPos.y > voxelEnd.y) ||
			(myEnd.y < voxelStart.y) ||
			(myPos.z > voxelEnd.z) ||
			(myEnd.z < voxelStart.z));
}
//Check for a collision with another physicsproxy
bool PhysicsActor::aabbCollision(PhysicsActor * other) {
	vec3 voxelStart = other->position - other->size*.5f;
	vec3 voxelEnd = other->position + other->size*.5f;
	vec3 myPos = position - size*.5f;
	vec3 myEnd = position + size*.5f;
	return !((myPos.x > voxelEnd.x) ||
			(myEnd.x < voxelStart.x) ||
			(myPos.y > voxelEnd.y) ||
			(myEnd.y < voxelStart.y) ||
			(myPos.z > voxelEnd.z) ||
			(myEnd.z < voxelStart.z));
}



void PhysicsActor::Draw(MaterialProgram * materialShader) {
	//Update model position
	if (model != NULL) 
		model->GetTransform().Translation() = vec3(position.x,position.y,position.z-size.z/2.0);
	//Update the underlying actor
	Actor::Draw(materialShader);
}