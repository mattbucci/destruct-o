#pragma once

#include "Weapon.h"
#include "WeaponData.h"
#include "WeaponModifiers.h"

class WeaponAI : public Weapon {
protected:
    //Whether or not the weapon should repeat firing automatically
    virtual bool RepeatFireAutomatically();

    //The amount of charge it takes to fire the weapon
    virtual float WeaponChargeRequired();

    //Cooldown length for the weapon
    virtual float WeaponCooldownTime();

    //The amount of jitter in the weapon
    virtual float JitterAmount();

    //Simulate a gun shot (or laser pulse or whatever)
    //also sets the jitter offsets for this pull
    virtual void Fire();

	//AI weapons do not use an animation lookup table
	string LookupAnimation(HandAnimations animation);

    //AI Weapon data
    WeaponData * data;
public:
    WeaponAI();

    virtual ~WeaponAI();

	//A set of weapon modifiers which change the way this weapon reacts
	WeaponModifiers Modifiers;

	//Allow any object to claim control of this weapon
	void SetOwner(PhysicsActor * owner);

    //Apply the given weapon data to this weapon
    virtual void ApplyData(WeaponData * weaponData);

    CLASS_DECLARATION(WeaponAI)
		INHERITS_FROM(Weapon)
        CLASS_MEMBER(data, ReflectionData::SAVE_OWNEDHANDLE)
		CLASS_MEMBER(Modifiers,ReflectionData::SAVE_INSTANCE)
    END_DECLARATION
};