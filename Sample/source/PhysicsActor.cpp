#include "stdafx.h"
#include "PhysicsActor.h"
#include "BaseFrame.h"

#include "ModelGroup.h"
#include "ModelInstance.h"
#include "Model.h"

PhysicsActor::PhysicsActor(FactionId faction)
    : Velocity(velocity), Position(position), Acceleration(acceleration), Size(size)
{
    size = vec3(1, 1, 1);
    maxLife = 100;
	energyPoolMax = 100;
	//Save settings
	this->faction = faction;
	//Apply defaults
	onGround = false;
	lastDamageRecieved = 0;
	lifeRegenRate = 100.0f/45.0f; //regenerate all health in 45 seconds by default
	life = maxLife;
	energyRegenRate = 10.0f; //regenerate all energy in 10 seconds
	energyPool = energyPoolMax;
	vulnerable = true;
	faction = GameFactions::FACTION_HOSTILE;
	flying = false;
	//Mark you're not colliding
	collidingWith = NULL;
	colliding = false;
	touchingWall = false;
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

//If you're touching anything, this is true
bool PhysicsActor::Colliding() {
	return colliding;
}
//If what you're touching is an actor, this is that actor
PhysicsActor * PhysicsActor::CollidingWith() {
	return collidingWith;
}

//Get the current life of this actor
float PhysicsActor::GetLife() {
	return life;
}

float PhysicsActor::GetLifePercentage() {
	return life/maxLife;
}

void PhysicsActor::SetFaction(FactionId faction) {
	this->faction = faction;
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
			//Can't die anymore
			vulnerable = false;
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
			//can't die anymore
			vulnerable = false;
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


//if the actor has experienced a sideways force from solid ground this is true
bool PhysicsActor::TouchingWall() {
	return touchingWall;
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


//Update life
bool PhysicsActor::Update() {
	//Regenerate life and energy while alive
	if (!Dead()) {
		life += SIMULATION_DELTA*lifeRegenRate;
		if (life > maxLife)
			life = maxLife;
		energyPool += SIMULATION_DELTA*energyRegenRate;
		if (energyPool > energyPoolMax)
			energyPool = energyPoolMax;
	}

	if ((life <= 0) && vulnerable) {
		//This unit died
		vulnerable = false;
		//call on death
		onDeath();
	}
	return Actor::Update();
}


void PhysicsActor::Draw(MaterialProgram * materialShader) {
	//Update model position
	if (model != NULL) 
		model->GetTransform().Translation() = vec3(position.x,position.y,position.z-size.z/2.0);

	//Update the underlying actor
	Actor::Draw(materialShader);
}