#pragma once

#include "Weapon.h"
#include "WeaponData.h"

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

    //AI Weapon data
    WeaponData * data;
public:
    WeaponAI();

    virtual ~WeaponAI();

    //Apply the given weapon data to this weapon
    virtual void Apply(WeaponData * weaponData);

    CLASS_DECLARATION(WeaponAI)
        CLASS_MEMBER(data, ReflectionData::SAVE_OWNEDHANDLE)
    END_DECLARATION
};