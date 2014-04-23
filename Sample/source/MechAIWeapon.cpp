#include "stdafx.h"
#include "MechAIWeapon.h"
#include "Universal.h"
#include "BaseFrame.h"

MechAIWeapon::MechAIWeapon(Actor * weaponOwner, float & chargePool) : Weapon(weaponOwner, chargePool), laser(vec4(1,.5,.1,1),.1f) {
	laser.SetTiming(.05f,1.0f,true);
}

//Whether or not the weapon should repeat firing automatically
bool MechAIWeapon::RepeatFireAutomatically() {
	return true;
}

//The amount of charge it takes to fire the weapon
float MechAIWeapon::WeaponChargeRequired() {
	return 0;
}

//Cooldown length for the weapon
float MechAIWeapon::WeaponCooldownTime() {
	return 2;
}

//Simulate a gun shot (or laser pulse or whatever)
void MechAIWeapon::Fire() {
	if (!Universal::Trace(firePointA,fireVector,&hitPos))
		hitPos = firePointA+fireVector*100.0f;
	
 	Universal::Concuss(hitPos,3,20,(PhysicsActor*)this->weaponOwner);
	laser.StartFiring();
	laser.Move(firePointA,hitPos);

	Weapon::Fire();
}


//Weapon firing animation
string MechAIWeapon::LookupAnimation(Weapon::HandAnimations animation) {
    static const string animations[10] = {
        //ANIMATION_MODELNAME,
        "robot02",
        //ANIMATION_AIM,
        "move_frozen",
        //ANIMATION_FIRE,
        "short_gun_burst",
        //ANIMATION_RELOAD,
        "move_frozen",
        //ANIMATION_RUN,
        "running",
        //ANIMATION_JUMP,
        "jump",
        //ANIMATION_GRENADE,
        "",
		//ANIMATION_DEATH
		"destroyed"
    };
    return animations[animation];
}

//Update the state of the weapon
void MechAIWeapon::Update(vec3 firingVector, vec3 firePointA, vec3 FirePointB) {
	Weapon::Update(firingVector, firePointA, FirePointB);
}

//Draw any effects the weapon may own
void MechAIWeapon::DrawWeapon(GLEffectProgram * shader, vec3 fireVector, vec3 firePointA, vec3 FirePointB) {
	//That's right I'm calling raytrace druing draw for the most up-to-date stuff possible
	if (!Universal::Trace(firePointA,fireVector,&hitPos))
		hitPos = firePointA+fireVector*100.0f;

	//Now move the laser to its new position and draw
	//laser.Move(firePoint,hitPos);
	laser.Draw(shader);
}