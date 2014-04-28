#include "stdafx.h"
#include "SoldierAIWeapon.h"
#include "Universal.h"
#include "BaseFrame.h"

CLASS_SAVE_CONSTRUCTOR(SoldierAIWeapon);

SoldierAIWeapon::SoldierAIWeapon() : Weapon(), laser(vec4(1,.5,.1,1),.1f) {
	laser.SetTiming(.05f,1.0f,true);
}

SoldierAIWeapon::SoldierAIWeapon(PhysicsActor * weaponOwner) : Weapon(weaponOwner), laser(vec4(1,.5,.1,1),.1f) {
	laser.SetTiming(.05f,1.0f,true);
}

//Whether or not the weapon should repeat firing automatically
bool SoldierAIWeapon::RepeatFireAutomatically() {
	return true;
}

//The amount of charge it takes to fire the weapon
float SoldierAIWeapon::WeaponChargeRequired() {
	return 0;
}

//Cooldown length for the weapon
float SoldierAIWeapon::WeaponCooldownTime() {
	return 2;
}

//The amount of jitter in the weapon
float SoldierAIWeapon::JitterAmount() {
	return .06f;
}

//Simulate a gun shot (or laser pulse or whatever)
void SoldierAIWeapon::Fire() {
	//Fire appropriate events and calculate jitter
	Weapon::Fire();
	//Apply jitter
	Weapon::updateFinalFireVectors();

	if (!Universal::Trace(firePointA,finalFireVectorA,&hitPos))
		hitPos = firePointA+finalFireVectorA*100.0f;
	
	weaponImpact(hitPos);

 	Universal::Concuss(hitPos,3,20,(PhysicsActor*)this->weaponOwner);
	laser.StartFiring();
	laser.Move(firePointA,hitPos);

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
string SoldierAIWeapon::LookupAnimation(Weapon::HandAnimations animation) {
    static const string animations[10] = {
        //ANIMATION_MODELNAME,
        "soldier01",
        //ANIMATION_AIM,
        "idleWgun",
        //ANIMATION_FIRE,
        "shooting",
        //ANIMATION_RELOAD,
        "Reload",
        //ANIMATION_RUN,
        "run",
        //ANIMATION_JUMP,
        "jump",
        //ANIMATION_GRENADE,
        "",
		//ANIMATION_DEATH
		"death"
    };
    return animations[animation];
}

//Update the state of the weapon
void SoldierAIWeapon::Update(vec3 firingVector, vec3 firePointA, vec3 firePointB) {
	Weapon::Update(firingVector, firePointA, firePointB);
}

//Draw any effects the weapon may own
void SoldierAIWeapon::DrawWeapon(GLEffectProgram * shader, vec3 fireVector, vec3 firePointA, vec3 firePointB) {
	//That's right I'm calling raytrace druing draw for the most up-to-date stuff possible
	if (!Universal::Trace(firePointA,fireVector,&hitPos))
		hitPos = firePointA+fireVector*100.0f;

	//Now move the laser to its new position and draw
	//laser.Move(firePoint,hitPos);
	laser.Draw(shader);
}