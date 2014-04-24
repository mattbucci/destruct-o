
#pragma once

#include "Weapon.h"
#include "EffectLaser.h"

class TurretAIWeapon : public Weapon {
protected:
	//Whether or not the weapon should repeat firing automatically
	virtual bool RepeatFireAutomatically() override;

	//The amount of charge it takes to fire the weapon
	virtual float WeaponChargeRequired() override;

	//Cooldown length for the weapon
	virtual float WeaponCooldownTime() override;

	//Simulate a gun shot (or laser pulse or whatever)
	virtual void Fire() override;

	//The amount of jitter in the weapon
	virtual float JitterAmount() override;

	//What you shoot
	EffectLaser laserA;
	EffectLaser laserB;
	vec3 hitPosA;
	vec3 hitPosB;
public:
	//DO NOT CALL
	TurretAIWeapon();

	//Call me
	TurretAIWeapon(PhysicsActor * weaponOwner);

	//Weapon firing animation
	virtual string LookupAnimation(HandAnimations animation) override;

	//Update the state of the weapon
	virtual void Update(vec3 firingVector, vec3 firePointA, vec3 FirePointB) override;

	//Draw any effects the weapon may own
	virtual void DrawWeapon(GLEffectProgram * shader, vec3 fireVector, vec3 firePointA, vec3 FirePointB) override;

	CLASS_DECLARATION(TurretAIWeapon)
		INHERITS_FROM(Weapon)
		CLASS_MEMBER(hitPosA, ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(hitPosB, ReflectionData::SAVE_VEC3)
	END_DECLARATION
};