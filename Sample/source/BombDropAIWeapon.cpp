#include "stdafx.h"
#include "BombDropAIWeapon.h"
#include "Universal.h"
#include "BaseFrame.h"
#include "ActorBomb.h"
#include "ActorSystem.h"

BombDropAIWeapon::BombDropAIWeapon(PhysicsActor * weaponOwner, float & chargePool) : Weapon(weaponOwner, chargePool) {
}

//Whether or not the weapon should repeat firing automatically
bool BombDropAIWeapon::RepeatFireAutomatically() {
	return true;
}

//The amount of charge it takes to fire the weapon
float BombDropAIWeapon::WeaponChargeRequired() {
	return 0;
}

//Cooldown length for the weapon
float BombDropAIWeapon::WeaponCooldownTime() {
	return .75;
}

//The amount of jitter in the weapon
float BombDropAIWeapon::JitterAmount() {
	return .08;
}

//Simulate a gun shot (or laser pulse or whatever)
void BombDropAIWeapon::Fire() {
	//Fire appropriate events and calculate jitter
	Weapon::Fire();
	//Apply jitter
	Weapon::updateFinalFireVectors();

	//Drop a bomb from both bomb ports
	ActorBomb * bombOne = Game()->Actors.BuildActor<ActorBomb>(firePointA);
	ActorBomb * bombTwo = Game()->Actors.BuildActor<ActorBomb>(firePointB);
	//Align the bombs to your faction
	bombOne->SetFaction(weaponOwner->GetFaction());
	bombTwo->SetFaction(weaponOwner->GetFaction());

	Weapon::Fire();
}



//Weapon firing animation
string BombDropAIWeapon::LookupAnimation(Weapon::HandAnimations animation) {
    static const string animations[10] = {
        //ANIMATION_MODELNAME,
        "dropship",
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
void BombDropAIWeapon::Update(vec3 firingVector, vec3 firePointA, vec3 firePointB) {
	Weapon::Update(firingVector, firePointA, firePointB);
}

//Draw any effects the weapon may own
void BombDropAIWeapon::DrawWeapon(GLEffectProgram * shader, vec3 fireVector, vec3 firePointA, vec3 firePointB) {
	//laser.Draw(shader);
}