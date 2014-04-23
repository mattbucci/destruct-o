#include "stdafx.h"
#include "BasicAIWeapon.h"
#include "Universal.h"
#include "BaseFrame.h"

BasicAIWeapon::BasicAIWeapon(Actor * weaponOwner, float & chargePool) : Weapon(weaponOwner, chargePool), laser(vec4(1,.5,.1,1),.1f) {
	laser.SetTiming(.05f,1.0f,true);
}

//Whether or not the weapon should repeat firing automatically
bool BasicAIWeapon::RepeatFireAutomatically() {
	return false;
}

//The amount of charge it takes to fire the weapon
float BasicAIWeapon::WeaponChargeRequired() {
	return 0;
}

//Cooldown length for the weapon
float BasicAIWeapon::WeaponCooldownTime() {
	return 2;
}

//Simulate a gun shot (or laser pulse or whatever)
void BasicAIWeapon::Fire() {
	if (!Universal::Trace(firePoint,fireVector,&hitPos))
		hitPos = firePoint+fireVector*100.0f;
	
 	Universal::Concuss(hitPos,3,20,(PhysicsActor*)this->weaponOwner);
	laser.StartFiring();
	laser.Move(firePoint,hitPos);

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
string BasicAIWeapon::LookupAnimation(Weapon::HandAnimations animation) {
    static const string animations[10] = {
        //ANIMATION_MODELNAME,
        "soldier01",
        //ANIMATION_AIM,
        "idleWgun",
        //ANIMATION_FIRE,
        "Fire1shot",
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
void BasicAIWeapon::Update(vec3 firingVector, vec3 firingPosition) {
	Weapon::Update(firingVector,firingPosition);
}

//Draw any effects the weapon may own
void BasicAIWeapon::DrawWeapon(GLEffectProgram * shader, vec3 fireVector, vec3 firePoint) {
	//That's right I'm calling raytrace druing draw for the most up-to-date stuff possible
	if (!Universal::Trace(firePoint,fireVector,&hitPos))
		hitPos = firePoint+fireVector*100.0f;

	//Now move the laser to its new position and draw
	//laser.Move(firePoint,hitPos);
	laser.Draw(shader);
}