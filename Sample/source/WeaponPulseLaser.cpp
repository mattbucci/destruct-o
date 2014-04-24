#include "stdafx.h"
#include "WeaponPulseLaser.h"
#include "Universal.h"
#include "BaseFrame.h"

WeaponPulseLaser::WeaponPulseLaser(Actor * weaponOwner, float & chargePool) : Weapon(weaponOwner, chargePool), laser(vec4(.1,.4,1,1),.1f) {
	laser.SetTiming(.05f,2.2f,true);
}

//Whether or not the weapon should repeat firing automatically
bool WeaponPulseLaser::RepeatFireAutomatically() {
	return true;
}

//The amount of charge it takes to fire the weapon
float WeaponPulseLaser::WeaponChargeRequired() {
	return 0;
}

//Cooldown length for the weapon
float WeaponPulseLaser::WeaponCooldownTime() {
	return .35;
}

//The amount of jitter in the weapon
float WeaponPulseLaser::JitterAmount(){
	return .05f;
}

//Simulate a gun shot (or laser pulse or whatever)
void WeaponPulseLaser::Fire() {
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
 	Universal::Concuss(hitPos,3,5,(PhysicsActor*)this->weaponOwner);

	//Update laser
	laser.StartFiring();
	laser.Move(firePointA,hitPos);
}
//Weapon firing animation
string WeaponPulseLaser::LookupAnimation(Weapon::HandAnimations animation) {
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
void WeaponPulseLaser::Update(vec3 firingVector, vec3 firePointA, vec3 firePointB) {
	Weapon::Update(firingVector, firePointA, firePointB);
}

//Draw any effects the weapon may own
void WeaponPulseLaser::DrawWeapon(GLEffectProgram * shader, vec3 fireVector, vec3 firePointA, vec3 firePointB) {
	this->fireVector = fireVector;
	Weapon::updateFinalFireVectors();

	//That's right I'm calling raytrace druing draw for the most up-to-date stuff possible
	if (!Universal::Trace(firePointA,finalFireVectorA,&hitPos))
		hitPos = firePointA+finalFireVectorA*100.0f;

	//Now move the laser to its new position and draw
	laser.Move(firePointA,hitPos);
	laser.Draw(shader);
}