#include "stdafx.h"
#include "EntityFireWeaponData.h"



CLASS_SAVE_CONSTRUCTOR(EntityFireWeaponData);



WeaponData * EntityFireWeaponData::Copy() {
    return new EntityFireWeaponData(*this);
}