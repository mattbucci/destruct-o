#include "stdafx.h"
#include "WeaponAI.h"
#include "Universal.h"

//Whether or not the weapon should repeat firing automatically
bool WeaponAI::RepeatFireAutomatically() {
    return data->RepeatFireAutomatically;
}
//The amount of charge it takes to fire the weapon
float WeaponAI::WeaponChargeRequired() {
    return data->ChargeRequired;
}
//Cooldown length for the weapon
float WeaponAI::WeaponCooldownTime() {
    return data->CooldownTime;
}
//The amount of jitter in the weapon
float WeaponAI::JitterAmount() {
    return data->JitterAmount;
}

//Simulate a gun shot (or laser pulse or whatever)
//also sets the jitter offsets for this pull
void WeaponAI::Fire() {
	Weapon::Fire();
}

//Allow any object to claim control of this weapon
void WeaponAI::SetOwner(PhysicsActor * owner) {
	weaponOwner = owner;
}

//AI weapons do not use an animation lookup table
string WeaponAI::LookupAnimation(HandAnimations animation) {
	_ASSERTE(false);
	return "";
}

WeaponAI::WeaponAI() {
    data = NULL;
}

WeaponAI::~WeaponAI() {
    delete data;
}

//Apply the given weapon data to this weapon
void WeaponAI::ApplyData(WeaponData * weaponData) {
    _ASSERTE(data == NULL);
    data = weaponData->Copy();
}