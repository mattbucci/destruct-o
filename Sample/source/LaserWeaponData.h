
#pragma once

#include "WeaponData.h"

class LaserWeaponData : public WeaponData {
public:
	//The thickness of the laser
	float LaserRadius;

	//The color of the laser
	vec4 LaserColor;

	//The time from pulling the trigger to full visual laser
	float LaserWarmUpTime;

	//The time from full laser to no laser
	float LaserCooldownTime;

	//Whether or not the laser continues to follow the muzzle after firing
	bool LaserFollowMuzzle;

    //Clone the laser weapon data
    virtual WeaponData * Copy() override;

	//Laser weapon save data
	CLASS_DECLARATION(LaserWeaponData)
		INHERITS_FROM(WeaponData)
		CLASS_MEMBER(LaserRadius,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(LaserColor,ReflectionData::SAVE_VEC4)
		CLASS_MEMBER(LaserWarmUpTime,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(LaserCooldownTime,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(LaserFollowMuzzle,ReflectionData::SAVE_BOOL)
	END_DECLARATION
};