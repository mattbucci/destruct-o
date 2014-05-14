#include "stdafx.h"
#include "WeaponModifiers.h"

CLASS_SAVE_CONSTRUCTOR(WeaponModifiers);

WeaponModifiers::WeaponModifiers() {
	DamageFactor = ChargeUseFactor = CooldownFactor = 1.0f;
}