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

	//Simulate a gun shot (or laser pulse or whatever)
	virtual void Fire();

	//Last time the weapon was fire
	double lastWeaponFire;
	//pool to draw charge from
	float & chargePool;
	//Whether or not they were holding the trigger last time
	bool lastHoldingTrigger;

	//the world information about the weapon position
	vec3 fireVector;
	vec3 firePoint;
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
    };

	//Weapon firing animation
    virtual string LookupAnimation(HandAnimations animation) = 0;

	//Whether or not the user is holding the trigger
	//returns whether or not the firing animation should play
	virtual bool HoldingTrigger(bool trigger);
	

	//Update the state of the weapon
	virtual void Update(vec3 fireVector, vec3 firePoint);

	//Draw any effects the weapon may own
	virtual void DrawWeapon(GLEffectProgram * shader, vec3 fireVector, vec3 firePoint);
};