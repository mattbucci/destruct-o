#include "stdafx.h"
#include "MechAIWeapon.h"
#include "Universal.h"
#include "BaseFrame.h"

MechAIWeapon::MechAIWeapon(Actor * weaponOwner, float & chargePool) : Weapon(weaponOwner, chargePool), laserA(vec4(1,.5,.1,1),.1f),laserB(vec4(1,.5,.1,1),.1f)  {
	laserA.SetTiming(.05f,2.2f,true);
	laserB.SetTiming(.05f,2.2f,true);
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

//The amount of jitter in the weapon
float MechAIWeapon::JitterAmount() {
	return .08;
}

//Simulate a gun shot (or laser pulse or whatever)
void MechAIWeapon::Fire() {
	//Fire appropriate events and calculate jitter
	Weapon::Fire();
	//Apply jitter
	Weapon::updateFinalFireVectors();

	//Do first fire point
	if (!Universal::TraceIgnoreActor(firePointA,finalFireVectorA,(PhysicsActor*)weaponOwner,&hitPosA))
		hitPosA = firePointA+finalFireVectorA*100.0f;
	
	weaponImpact(hitPosA);
 	Universal::Concuss(hitPosA,3,20,(PhysicsActor*)this->weaponOwner);
	//Do second fire point
	if (!Universal::TraceIgnoreActor(firePointB,finalFireVectorB,(PhysicsActor*)weaponOwner,&hitPosB))
		hitPosB = firePointB+finalFireVectorB*100.0f;
	
	weaponImpact(hitPosB);
 	Universal::Concuss(hitPosB,3,20,(PhysicsActor*)this->weaponOwner);


	laserA.StartFiring();
	laserA.Move(firePointB,hitPosB);

	laserB.StartFiring();
	laserB.Move(firePointB,hitPosB);
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
void MechAIWeapon::DrawWeapon(GLEffectProgram * shader, vec3 fireVector, vec3 firePointA, vec3 firePointB) {
	//That's right I'm calling raytrace druing draw for the most up-to-date stuff possible
	//if (!Universal::Trace(firePointB,fireVector,&hitPosB))
	//	hitPosB = firePointB+fireVector*100.0f;

	//Now move the laser to its new position and draw
	laserA.Move(firePointA,hitPosA);
	laserA.Draw(shader);
	laserB.Move(firePointB,hitPosB);
	laserB.Draw(shader);
}