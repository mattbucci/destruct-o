
#pragma once

#include "stdafx.h"

typedef enum : int {
	WEAPONBEHAVIOR_LASER,
	WEAPONBEHAVIOR_ENTITYFIRE, //bomb drop, missiles, rpg?
	WEAPONBEHAVIOR_ENTITYEXPLODE, //the bomb itself, the missile itself, etc.
} WeaponBehaviors;

class WeaponData : public Savable{
public:
	//setup defaults
	WeaponData();

	//Save a default, used for debug purposes only
	static void SaveDefault();

	//The behavior of this weapon
	WeaponBehaviors Behavior;

	//Whether or not this weapon is a pair of weapons
	bool DualWeapon;

	//Whether or not the weapon should repeat firing automatically
	bool RepeatFireAutomatically;

	//The amount of charge it takes to fire the weapon
	float ChargeRequired;

	//Cooldown length for the weapon
	float CooldownTime;

	//The amount of jitter in the weapon
	float JitterAmount;

	//The amount of damage done
	float Damage;

	//The radius of the damage
	float Radius;

	//The particles which represent the weapon striking a surface
	string WeaponHitParticles;

	//The particles which represent the wepaon firing
	string MuzzleFlashParticles;

	//All actor ai data
	CLASS_DECLARATION(WeaponData)
		CLASS_MEMBER(Behavior,ReflectionData::SAVE_INT32)
		CLASS_MEMBER(DualWeapon,ReflectionData::SAVE_BOOL)
		CLASS_MEMBER(RepeatFireAutomatically,ReflectionData::SAVE_BOOL)
		CLASS_MEMBER(ChargeRequired,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(CooldownTime,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(JitterAmount,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(Damage,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(Radius,ReflectionData::SAVE_FLOAT)
	END_DECLARATION
};