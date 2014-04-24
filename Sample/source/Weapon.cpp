#include "stdafx.h"
#include "Weapon.h"
#include "BaseFrame.h"

Weapon::Weapon() {
	lastHoldingTrigger = false;
	weaponOwner = NULL;
	lastWeaponFire = 0;
}

Weapon::Weapon(PhysicsActor * weaponOwner) {
	lastHoldingTrigger = false;
	this->weaponOwner = weaponOwner;
	lastWeaponFire = 0;
}
Weapon::~Weapon() {

}

void Weapon::Fire() {
	//Decrease the charge
	weaponOwner->EnergyPool() -= WeaponChargeRequired();
	//Mark the gun as fired
	lastWeaponFire = Game()->Now();
	//Run the weapon fired event
	Game()->Actors.ActorFiredWeapon.Fire([this](function<void(Actor*,Weapon*)> observer) {
		observer(weaponOwner,this);
	});
	//Create weapon jitter for this shot
	float min = -JitterAmount();
	float max = JitterAmount();
	weaponJitterA = vec3(Utilities::random(min,max),Utilities::random(min,max),Utilities::random(min,max));
	weaponJitterB = vec3(Utilities::random(min,max),Utilities::random(min,max),Utilities::random(min,max));
}

//Run the weapon impact event with the given hit position
void Weapon::weaponImpact(vec3 hitAt) {
	Game()->Actors.ActorWeaponImpact.Fire([this,hitAt](function<void(Actor*,Weapon*,vec3)> observer) {
		observer(weaponOwner,this,hitAt);
	});
}

void Weapon::updateFinalFireVectors() {
	finalFireVectorA = glm::normalize(fireVector+weaponJitterA);
	finalFireVectorB = glm::normalize(fireVector+weaponJitterB);
}

//Whether or not the user is holding the trigger
bool Weapon::HoldingTrigger(bool trigger) {
	//If they arent' holding the trigger stop
	if (!trigger) {
		lastHoldingTrigger = false;
		return false;
	}
	bool prev = lastHoldingTrigger;
	lastHoldingTrigger = true;
	//Check if cooldown is over
	if ((lastWeaponFire != 0) && (Game()->Now() < (lastWeaponFire+WeaponCooldownTime())))
		return false;

	//Check if you have sufficient charge
	if (weaponOwner->EnergyPool() < WeaponChargeRequired()) {
		//Fire the event that you're out of charge
		if (!prev) {
			Game()->Actors.ActorFiredWhileEmpty.Fire([this](function<void(Actor*,Weapon*)> observer) {
				observer(weaponOwner,this);
			});
		}
		return false;
	}
		

	//Check if you meet the automatic requirement
	if ((!RepeatFireAutomatically()) && prev)
		return false;
	
	//Ok you can fire
	Fire();
	return true;
}

//Update the state of the weapon
void Weapon::Update(vec3 fireVector, vec3 firePointA, vec3 firePointB) {
	this->firePointA = firePointA;
	this->firePointB = firePointB;
	this->fireVector = fireVector;
}

//Draw any effects the weapon may own
void Weapon::DrawWeapon(GLEffectProgram * shader, vec3 fireVector, vec3 firePointA, vec3 firePointB) {

}