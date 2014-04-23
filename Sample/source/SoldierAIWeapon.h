
#pragma once

#include "Weapon.h"
#include "EffectLaser.h"

class SoldierAIWeapon : public Weapon {
protected:
	//Whether or not the weapon should repeat firing automatically
	virtual bool RepeatFireAutomatically() override;

	//The amount of charge it takes to fire the weapon
	virtual float WeaponChargeRequired() override;

	//Cooldown length for the weapon
	virtual float WeaponCooldownTime() override;

	//Simulate a gun shot (or laser pulse or whatever)
	virtual void Fire() override;

	//What you shoot
	EffectLaser laser;
	vec3 hitPos;
public:
	SoldierAIWeapon(Actor * weaponOwner, float & chargePool);

	//Weapon firing animation
	virtual string LookupAnimation(HandAnimations animation) override;

	//Update the state of the weapon
	virtual void Update(vec3 firingVector, vec3 firePointA, vec3 firePointB) override;

	//Draw any effects the weapon may own
	virtual void DrawWeapon(GLEffectProgram * shader, vec3 fireVector, vec3 firePointA, vec3 firePointB) override;
};