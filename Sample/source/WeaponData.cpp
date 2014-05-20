#include "stdafx.h"
#include "WeaponData.h"
#include "lodepng.h"

CLASS_SAVE_CONSTRUCTOR(WeaponData);


//setup defaults
WeaponData::WeaponData() {
	Behavior = WEAPONBEHAVIOR_LASER;
	DualWeapon = false;
	RepeatFireAutomatically = false;
	ChargeRequired = 0;
	CooldownTime = 1.0f;
	JitterAmount = .01f;
	Damage = 100.0f;
	Radius = 2.0f;
}

//Save a default, used for debug purposes only
void WeaponData::SaveDefault() {
	WeaponData data;
	vector<unsigned char> fileData = Savable::Serialize(&data,false);
	lodepng::save_file(fileData,"defaultweapon.json");
}

//Copy this weapon data instance
WeaponData * WeaponData::Copy() {
    return new WeaponData(*this);
}