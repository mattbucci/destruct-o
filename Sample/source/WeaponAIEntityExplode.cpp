#include "stdafx.h"
#include "WeaponAIEntityExplode.h"
#include "Universal.h"
#include "BaseFrame.h"

WeaponAIEntityExplode::WeaponAIEntityExplode() : WeaponAI() {
}


//Simulate a gun shot (or laser pulse or whatever)
void WeaponAIEntityExplode::Fire() {
    //Fire appropriate events and calculate jitter
    Weapon::Fire();
    //Apply jitter
    Weapon::updateFinalFireVectors();

	//Launch one or more projectiles

    Weapon::Fire();
}
