
#pragma once

#include "Weapon.h"
#include "EffectLaser.h"

class BombExplodeEffectWeapon : public Weapon {
protected:
	//Whether or not the weapon should repeat firing automatically
	virtual bool RepeatFireAutomatically() override;

	//The amount of charge it takes to fire the weapon
	virtual float WeaponChargeRequired() override;

	//Cooldown length for the weapon
	virtual float WeaponCooldownTime() override;

	//The amount of jitter in the weapon
	virtual float JitterAmount() override;

	//Simulate a gun shot (or laser pulse or whatever)
	virtual void Fire() override;

	//What you shoot
	vec3 hitPos;
public:
	BombExplodeEffectWeapon(PhysicsActor * weaponOwner, float & chargePool);

	//Weapon firing animation
	virtual string LookupAnimation(HandAnimations animation) override;

	//Update the state of the weapon
	virtual void Update(vec3 firingVector, vec3 firePointA, vec3 firePointB) override;

	//Draw any effects the weapon may own
	virtual void DrawWeapon(GLEffectProgram * shader, vec3 fireVector, vec3 firePointA, vec3 firePointB) override;
};