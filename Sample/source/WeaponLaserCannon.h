
#pragma once

#include "Weapon.h"
#include "EffectLaser.h"

class WeaponLaserCannon : public Weapon {
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
	EffectLaser laser;
	vec3 hitPos;
public:
	//DO NOT CALL
	WeaponLaserCannon();

	//Call me
	WeaponLaserCannon(PhysicsActor * weaponOwner);

	//Weapon firing animation
    virtual string LookupAnimation(HandAnimations animation) override;

	//Update the state of the weapon
	virtual void Update(vec3 firingVector, vec3 firePointA, vec3 firePointB = vec3()) override;

	//Draw any effects the weapon may own
	virtual void DrawWeapon(GLEffectProgram * shader, vec3 fireVector, vec3 firePointA, vec3 firePointB = vec3()) override;

	CLASS_DECLARATION(WeaponLaserCannon)
		INHERITS_FROM(Weapon)
		CLASS_MEMBER(hitPos, ReflectionData::SAVE_VEC3)
	END_DECLARATION
};