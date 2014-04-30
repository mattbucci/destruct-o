
#pragma once

#include "WeaponAI.h"
#include "EffectLaser.h"

class LaserWeaponData;

class WeaponAILaser : public WeaponAI {
protected:
	LaserWeaponData * laserData;

    //Simulate a gun shot (or laser pulse or whatever)
    //also sets the jitter offsets for this pull
    virtual void Fire();

	//What you shoot
	EffectLaser laserA;
	EffectLaser laserB;
	vec3 hitPosA;
	vec3 hitPosB;
public:
	//setup laser
	WeaponAILaser();
	
	//Update the state of the weapon
	virtual void Update(vec3 firingVector, vec3 firePointA, vec3 FirePointB) override;

	//Draw any effects the weapon may own
	virtual void DrawWeapon(GLEffectProgram * shader, vec3 fireVector, vec3 firePointA, vec3 FirePointB) override;

	//Apply the given weapon data to this weapon
    virtual void ApplyData(WeaponData * weaponData) override;

	CLASS_DECLARATION(WeaponAILaser)
		INHERITS_FROM(Weapon)
		CLASS_MEMBER(hitPosA, ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(hitPosB, ReflectionData::SAVE_VEC3)
	END_DECLARATION
};