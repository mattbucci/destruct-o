#include "stdafx.h"
#include "BombExplodeEffectWeapon.h"
#include "Universal.h"
#include "BaseFrame.h"

BombExplodeEffectWeapon::BombExplodeEffectWeapon(PhysicsActor * weaponOwner, float & chargePool) : Weapon(weaponOwner, chargePool) {
}

//Whether or not the weapon should repeat firing automatically
bool BombExplodeEffectWeapon::RepeatFireAutomatically() {
	return false;
}

//The amount of charge it takes to fire the weapon
float BombExplodeEffectWeapon::WeaponChargeRequired() {
	return 0;
}

//Cooldown length for the weapon
float BombExplodeEffectWeapon::WeaponCooldownTime() {
	return 100000;
}

//The amount of jitter in the weapon
float BombExplodeEffectWeapon::JitterAmount() {
	return .08;
}

//Simulate a gun shot (or laser pulse or whatever)
void BombExplodeEffectWeapon::Fire() {
	//Fire appropriate events and calculate jitter
	Weapon::Fire();
	//Apply jitter
	Weapon::updateFinalFireVectors();

	//Blow up where you stand
	weaponImpact(firePointA);
	
 	Universal::Concuss(firePointA,4,40,(PhysicsActor*)this->weaponOwner);

	Weapon::Fire();
}

/*
	static const string table[6] = {
		//ANIMATION_IDLE
		"idleWgun",
		//ANIMATION_AIM
		"",
		//ANIMATION_WALK
		"run",
		//ANIMATION_JUMP
		"jump",
		//ANIMATION_DEATH
		"death",
		//ANIMATION_INAIR
		"idleWgun"
	};
	*/


//Weapon firing animation
string BombExplodeEffectWeapon::LookupAnimation(Weapon::HandAnimations animation) {
    static const string animations[10] = {
        //ANIMATION_MODELNAME,
        "dropship01",
        //ANIMATION_AIM,
        "",
        //ANIMATION_FIRE,
        "",
        //ANIMATION_RELOAD,
        "",
        //ANIMATION_RUN,
        "",
        //ANIMATION_JUMP,
        "",
        //ANIMATION_GRENADE,
        "",
		//ANIMATION_DEATH
		""
    };
    return animations[animation];
}

//Update the state of the weapon
void BombExplodeEffectWeapon::Update(vec3 firingVector, vec3 firePointA, vec3 firePointB) {
	Weapon::Update(firingVector, firePointA, firePointB);
}

//Draw any effects the weapon may own
void BombExplodeEffectWeapon::DrawWeapon(GLEffectProgram * shader, vec3 fireVector, vec3 firePointA, vec3 firePointB) {
	//laser.Draw(shader);
}