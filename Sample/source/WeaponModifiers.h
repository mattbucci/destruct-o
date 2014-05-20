#pragma once

#include "stdafx.h"

class WeaponModifiers : public Savable {
public:
	WeaponModifiers();

	//The factor with which to increase/decrease cooldown (1.1 is a 10% increase)
	float CooldownFactor;

	//The factor with which to increase/decrease charge use (1.1 is a 10% increase)
	float ChargeUseFactor;

	//The factor with which to increase/decrease damage (1.1 is a 10% increase)
	float DamageFactor;

	CLASS_DECLARATION(WeaponModifiers)
		CLASS_MEMBER(CooldownFactor,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(ChargeUseFactor,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(DamageFactor,ReflectionData::SAVE_FLOAT)
	END_DECLARATION
};