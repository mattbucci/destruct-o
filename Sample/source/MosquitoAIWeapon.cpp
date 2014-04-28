#include "stdafx.h"
#include "MosquitoAIWeapon.h"
#include "Universal.h"
#include "BaseFrame.h"

CLASS_SAVE_CONSTRUCTOR(MosquitoAIWeapon);

MosquitoAIWeapon::MosquitoAIWeapon() : Weapon() {

}

MosquitoAIWeapon::MosquitoAIWeapon(PhysicsActor * weaponOwner) : Weapon(weaponOwner) {
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
	return .1f;
}

//The amount of jitter in the weapon
float MosquitoAIWeapon::JitterAmount() {
	return .08f;
}

//Simulate a gun shot (or laser pulse or whatever)
void MosquitoAIWeapon::Fire() {
	//Fire appropriate events and calculate jitter
	Weapon::Fire();
	//Apply jitter
	Weapon::updateFinalFireVectors();

	if (!Universal::Trace(firePointA,finalFireVectorA,&hitPos))
		hitPos = firePointA+finalFireVectorA*100.0f;
	
	weaponImpact(hitPos);
	
 	Universal::Concuss(hitPos,1,2,(PhysicsActor*)this->weaponOwner);

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
        "helicopter",
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
	//laser.Draw(shader);
}