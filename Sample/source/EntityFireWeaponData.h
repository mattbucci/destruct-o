
#pragma once

#include "WeaponData.h"

class EntityFireWeaponData : public WeaponData {

	//The name of the actor this weapon fires
	string ActorFired;

	//The velocity to impart on fired actors
	float Velocity;

	//Entity weapon save data
	CLASS_DECLARATION(EntityFireWeaponData)
		INHERITS_FROM(WeaponData)
		CLASS_MEMBER(ActorFired,ReflectionData::SAVE_STRING)
		CLASS_MEMBER(Velocity,ReflectionData::SAVE_FLOAT)
	END_DECLARATION
};