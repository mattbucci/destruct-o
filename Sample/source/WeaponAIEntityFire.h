
#pragma once

#include "WeaponAI.h"
#include "EffectLaser.h"

class WeaponAIEntityFire : public WeaponAI {
protected:
	//Simulate missile launch
	virtual void Fire() override;

public:

	CLASS_DECLARATION(WeaponAIEntityFire)
		INHERITS_FROM(WeaponAI)
	END_DECLARATION
};