#include "stdafx.h"
#include "MosquitoAIWeapon.h"
#include "Universal.h"
#include "BaseFrame.h"

MosquitoAIWeapon::MosquitoAIWeapon(Actor * weaponOwner, float & chargePool) : Weapon(weaponOwner, chargePool), laser(vec4(1,.5,.1,1),.1f) {
	laser.SetTiming(.05f,1.0f,true);
}

//Whether or not the weapon should repeat firing automatically
bool MosquitoAIWeapon::RepeatFireAutomatically() {
	return true;
}

//The amount of charge it takes to fire the weapon
float MosquitoAIWeapon::WeaponChargeRequired() {
	return 0;
}

//Cooldown length for the weapon
float MosquitoAIWeapon::WeaponCooldownTime() {
	return .5;
}

//The amount of jitter in the weapon
float MosquitoAIWeapon::JitterAmount() {
	return .09;
}

//Simulate a gun shot (or laser pulse or whatever)
void MosquitoAIWeapon::Fire() {
	if (!Universal::Trace(firePointA,fireVector,&hitPos))
		hitPos = firePointA+fireVector*100.0f;
	
 	Universal::Concuss(hitPos,1.5,5,(PhysicsActor*)this->weaponOwner);
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
string MosquitoAIWeapon::LookupAnimation(Weapon::HandAnimations animation) {
    static const string animations[10] = {
        //ANIMATION_MODELNAME,
        "mosquito",
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
void MosquitoAIWeapon::Update(vec3 firingVector, vec3 firePointA, vec3 firePointB) {
	Weapon::Update(firingVector, firePointA, firePointB);
}

//Draw any effects the weapon may own
void MosquitoAIWeapon::DrawWeapon(GLEffectProgram * shader, vec3 fireVector, vec3 firePointA, vec3 firePointB) {
	laser.Draw(shader);
}