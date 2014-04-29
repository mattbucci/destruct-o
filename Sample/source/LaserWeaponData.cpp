#include "stdafx.h"
#include "LaserWeaponData.h"


CLASS_SAVE_CONSTRUCTOR(LaserWeaponData);


WeaponData * LaserWeaponData::Copy() {
    return new LaserWeaponData(*this);
}