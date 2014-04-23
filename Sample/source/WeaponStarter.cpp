#include "stdafx.h"
#include "WeaponStarter.h"
#include "Universal.h"
#include "BaseFrame.h"

WeaponStarter::WeaponStarter(Actor * weaponOwner, float & chargePool) : Weapon(weaponOwner, chargePool), laser(vec4(1,.5,.1,1),.1f) {
	laser.SetTiming(.05f,1.0f,true);
}

//Whether or not the weapon should repeat firing automatically
bool WeaponStarter::RepeatFireAutomatically() {
	return false;
}

//The amount of charge it takes to fire the weapon
float WeaponStarter::WeaponChargeRequired() {
	return 0;
}

//Cooldown length for the weapon
float WeaponStarter::WeaponCooldownTime() {
	return 2;
}

//Simulate a gun shot (or laser pulse or whatever)
void WeaponStarter::Fire() {
	if (!Universal::Trace(firePointA,fireVector,&hitPos))
		hitPos = firePointA+fireVector*100.0f;
	
 	Universal::Concuss(hitPos,3,20,(PhysicsActor*)this->weaponOwner);
	laser.StartFiring();
	laser.Move(firePointA,hitPos);

	Weapon::Fire();
}
//Weapon firing animation
string WeaponStarter::LookupAnimation(Weapon::HandAnimations animation) {
    static const string animations[10] = {
        //ANIMATION_MODELNAME,
        "player_weapon",
        //ANIMATION_AIM,
        "Idle",
        //ANIMATION_FIRE,
        "Fire1shot",
        //ANIMATION_RELOAD,
        "Reload",
        //ANIMATION_RUN,
        "Run",
        //ANIMATION_JUMP,
        "Idle",
        //ANIMATION_GRENADE,
        "Granade",
		//ANIMATION_DEATH
		"Death"
    };
    return animations[animation];
}

//Update the state of the weapon
void WeaponStarter::Update(vec3 firingVector, vec3 firePointA, vec3 firePointB) {
	Weapon::Update(firingVector, firePointA, firePointB);
}

//Draw any effects the weapon may own
void WeaponStarter::DrawWeapon(GLEffectProgram * shader, vec3 fireVector, vec3 firePointA, vec3 firePointB) {
	//That's right I'm calling raytrace druing draw for the most up-to-date stuff possible
	if (!Universal::Trace(firePointA,fireVector,&hitPos))
		hitPos = firePointA+fireVector*100.0f;

	//Now move the laser to its new position and draw
	//laser.Move(firePoint,hitPos);
	laser.Draw(shader);
}