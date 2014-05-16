
#pragma once

#include "WeaponAI.h"
#include "EffectLaser.h"

class WeaponAIEntityExplode : public WeaponAI {
protected:
	//Simulate a gun shot (or laser pulse or whatever)
	virtual void Fire() override;


public:
	WeaponAIEntityExplode();

	CLASS_DECLARATION(WeaponAIEntityExplode)
		INHERITS_FROM(WeaponAI)
	END_DECLARATION
};