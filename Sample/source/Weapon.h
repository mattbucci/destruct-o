#pragma once

#include "stdafx.h"

class GLEffectProgram;
class Actor;

class Weapon {
protected:
	Actor * weaponOwner;

	//Whether or not the weapon should repeat firing automatically
	virtual bool RepeatFireAutomatically() = 0;

	//The amount of charge it takes to fire the weapon
	virtual float WeaponChargeRequired() = 0;

	//Cooldown length for the weapon
	virtual float WeaponCooldownTime() = 0;

	//The amount of jitter in the weapon
	virtual float JitterAmount() = 0;

	//Simulate a gun shot (or laser pulse or whatever)
	//also sets the jitter offsets for this pull
	virtual void Fire();

	//Mixes jitter into the fireVector producing finalFireVectorA/B
	void updateFinalFireVectors();

	//Run the weapon impact event with the given hit position
	void weaponImpact(vec3 hitAt);

	//Last time the weapon was fire
	double lastWeaponFire;
	//pool to draw charge from
	float & chargePool;
	//Whether or not they were holding the trigger last time
	bool lastHoldingTrigger;

	//the world information about the weapon position
	vec3 fireVector;
	vec3 firePointA;
	vec3 firePointB;
	//The final fire vector (initial vector + jitter offset) for each fire
	//updated during updateFinalFireVectors()
	vec3 finalFireVectorA;
	vec3 finalFireVectorB;
	//weapon jitter offsets
	vec3 weaponJitterA;
	vec3 weaponJitterB;
public:
	Weapon(Actor * weaponOwner, float & chargePool);
	virtual ~Weapon();

    enum HandAnimations {
        ANIMATION_MODELNAME = 0,
        ANIMATION_AIM,
        ANIMATION_FIRE,
        ANIMATION_RELOAD,
        ANIMATION_RUN,
        ANIMATION_JUMP,
        ANIMATION_GRENADE,
		ANIMATION_DEATH,
    };

	//Weapon firing animation
    virtual string LookupAnimation(HandAnimations animation) = 0;

	//Whether or not the user is holding the trigger
	//returns whether or not the firing animation should play
	virtual bool HoldingTrigger(bool trigger);
	

	//Update the state of the weapon
	virtual void Update(vec3 fireVector, vec3 firePointA, vec3 FirePointB = vec3());

	//Draw any effects the weapon may own
	virtual void DrawWeapon(GLEffectProgram * shader, vec3 fireVector, vec3 firePointA, vec3 FirePointB = vec3());
};