#include "stdafx.h"
#include "WeaponAIEntityFire.h"
#include "Universal.h"
#include "BaseFrame.h"
#include "ActorBomb.h"
#include "ActorSystem.h"
#include "EntityFireWeaponData.h"
#include "ActorProjectile.h"

CLASS_SAVE_CONSTRUCTOR(WeaponAIEntityFire);


//Simulate a gun shot (or laser pulse or whatever)
void WeaponAIEntityFire::Fire() {
	EntityFireWeaponData * fireData = (EntityFireWeaponData*)data;
	_ASSERTE(dynamic_cast<EntityFireWeaponData*>(data) != NULL);

	//Fire appropriate events and calculate jitter
	Weapon::Fire();
	//Apply jitter
	Weapon::updateFinalFireVectors();

	//Create an entity at one or both locations
	ActorProjectile * bombOne = dynamic_cast<ActorProjectile*>(Game()->Actors.BuildActorFromName(fireData->ActorFired));
	//Better be a projectile or how can it be a projectile?
	_ASSERTE(bombOne != NULL);
	
	bombOne->FireProjectile(this->weaponOwner,firePointA,finalFireVectorA*fireData->Velocity);

	//Align the bombs to your faction
	bombOne->SetFaction(weaponOwner->GetFaction());

	if (fireData->DualWeapon) {
		//Create an entity at one or both locations
		ActorProjectile * bombTwo = dynamic_cast<ActorProjectile*>(Game()->Actors.BuildActorFromName(fireData->ActorFired));
		//Better be a projectile or how can it be a projectile?
		_ASSERTE(bombTwo != NULL);

		bombTwo->FireProjectile(this->weaponOwner,firePointB,finalFireVectorB*fireData->Velocity);

		bombTwo->SetFaction(weaponOwner->GetFaction());
	}

	//TODO: Fix velocity vector of fired weapons



	Weapon::Fire();
}

