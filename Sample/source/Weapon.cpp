#include "stdafx.h"
#include "Weapon.h"
#include "BaseFrame.h"

Weapon::Weapon(Actor * weaponOwner, float & pool) : chargePool(pool) {
	lastHoldingTrigger = false;
	this->weaponOwner = weaponOwner;
}
Weapon::~Weapon() {

}

void Weapon::Fire() {
	//Decrease the charge
	chargePool -= WeaponChargeRequired();
	//Mark the gun as fired
	lastWeaponFire = Game()->Now();
	//Run the weapon fired event
	Game()->Actors.ActorFiredWeapon.Fire([this](function<void(Actor*,Weapon*)> observer) {
		observer(weaponOwner,this);
	});
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
	if (Game()->Now() < (lastWeaponFire+WeaponCooldownTime()))
		return false;

	//Check if you have sufficient charge
	if (chargePool < WeaponChargeRequired()) {
		//Fire the event that you're out of charge
		if (!lastHoldingTrigger) {
			Game()->Actors.ActorFiredWhileEmpty.Fire([this](function<void(Actor*,Weapon*)> observer) {
				observer(weaponOwner,this);
			});
		}
		return false;
	}
		

	//Check if you meet the automatic requirement
	if ((!RepeatFireAutomatically()) && lastHoldingTrigger)
		return false;
	
	//Ok you can fire
	Fire();
	return true;
}

//Update the state of the weapon
void Weapon::Update(vec3 fireVector, vec3 firePoint) {
	this->firePoint = firePoint;
	this->fireVector = fireVector;
}

//Draw any effects the weapon may own
void Weapon::DrawWeapon(GLEffectProgram * shader, vec3 fireVector, vec3 firePoint) {

}