#include "stdafx.h"
#include "WeaponLaserCannon.h"
#include "Universal.h"
#include "BaseFrame.h"

WeaponLaserCannon::WeaponLaserCannon(PhysicsActor * weaponOwner, float & chargePool) : Weapon(weaponOwner, chargePool), laser(vec4(1,.5,.1,1),.1f) {
	laser.SetTiming(.05f,1.0f,true);
}

//Whether or not the weapon should repeat firing automatically
bool WeaponLaserCannon::RepeatFireAutomatically() {
	return false;
}

//The amount of charge it takes to fire the weapon
float WeaponLaserCannon::WeaponChargeRequired() {
	return 0;
}

//Cooldown length for the weapon
float WeaponLaserCannon::WeaponCooldownTime() {
	return 2;
}

//The amount of jitter in the weapon
float WeaponLaserCannon::JitterAmount(){
	return .02f;
}

//Simulate a gun shot (or laser pulse or whatever)
void WeaponLaserCannon::Fire() {
	//Fire appropriate events and calculate jitter
	Weapon::Fire();
	//Apply jitter
	Weapon::updateFinalFireVectors();

	//Determine weapon hit location
	if (!Universal::Trace(firePointA,finalFireVectorA,&hitPos))
		hitPos = firePointA+finalFireVectorA*100.0f;
	
	//Fire event
	weaponImpact(hitPos);
	//Do damage
 	Universal::Concuss(hitPos,3,20,(PhysicsActor*)this->weaponOwner);

	//Update laser
	laser.StartFiring();
	laser.Move(firePointA,hitPos);
}
//Weapon firing animation
string WeaponLaserCannon::LookupAnimation(Weapon::HandAnimations animation) {
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
void WeaponLaserCannon::Update(vec3 firingVector, vec3 firePointA, vec3 firePointB) {
	Weapon::Update(firingVector, firePointA, firePointB);
}

//Draw any effects the weapon may own
void WeaponLaserCannon::DrawWeapon(GLEffectProgram * shader, vec3 fireVector, vec3 firePointA, vec3 firePointB) {
	//Draw the laser
	laser.Draw(shader);
}