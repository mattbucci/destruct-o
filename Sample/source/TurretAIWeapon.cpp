#include "stdafx.h"
#include "TurretAIWeapon.h"
#include "Universal.h"
#include "BaseFrame.h"

TurretAIWeapon::TurretAIWeapon(PhysicsActor * weaponOwner, float & chargePool) : Weapon(weaponOwner, chargePool), laserA(vec4(1,.5,.1,1),.1f),laserB(vec4(1,.5,.1,1),.1f)  {
	laserA.SetTiming(.05f,2.2f,true);
	laserB.SetTiming(.05f,2.2f,true);
}

//Whether or not the weapon should repeat firing automatically
bool TurretAIWeapon::RepeatFireAutomatically() {
	return true;
}

//The amount of charge it takes to fire the weapon
float TurretAIWeapon::WeaponChargeRequired() {
	return 0;
}

//Cooldown length for the weapon
float TurretAIWeapon::WeaponCooldownTime() {
	return 2;
}

//The amount of jitter in the weapon
float TurretAIWeapon::JitterAmount() {
	return .08;
}

//Simulate a gun shot (or laser pulse or whatever)
void TurretAIWeapon::Fire() {
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
string TurretAIWeapon::LookupAnimation(Weapon::HandAnimations animation) {
    static const string animations[10] = {
        //ANIMATION_MODELNAME,
        "turret1",
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
void TurretAIWeapon::Update(vec3 firingVector, vec3 firePointA, vec3 FirePointB) {
	Weapon::Update(firingVector, firePointA, FirePointB);
}

//Draw any effects the weapon may own
void TurretAIWeapon::DrawWeapon(GLEffectProgram * shader, vec3 fireVector, vec3 firePointA, vec3 firePointB) {
	//That's right I'm calling raytrace druing draw for the most up-to-date stuff possible
	//if (!Universal::Trace(firePointB,fireVector,&hitPosB))
	//	hitPosB = firePointB+fireVector*100.0f;

	//Now move the laser to its new position and draw
	laserA.Move(firePointA,hitPosA);
	laserA.Draw(shader);
	laserB.Move(firePointB,hitPosB);
	laserB.Draw(shader);
}