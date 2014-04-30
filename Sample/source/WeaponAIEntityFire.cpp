#include "stdafx.h"
#include "WeaponAIEntityFire.h"
#include "Universal.h"
#include "BaseFrame.h"
#include "ActorBomb.h"
#include "ActorSystem.h"
#include "EntityFireWeaponData.h"

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
	PhysicsActor * bombOne = dynamic_cast<PhysicsActor*>(Game()->Actors.BuildActorFromName(fireData->ActorFired));
	//Better be a physics actor or how can it be ap rojectile?
	_ASSERTE(bombOne != NULL);

	//Align the bombs to your faction
	bombOne->SetFaction(weaponOwner->GetFaction());

	if (fireData->DualWeapon) {
		//Create an entity at one or both locations
		PhysicsActor * bombTwo = dynamic_cast<PhysicsActor*>(Game()->Actors.BuildActorFromName(fireData->ActorFired));
		//Better be a physics actor or how can it be ap rojectile?
		_ASSERTE(bombTwo != NULL);

		bombTwo->SetFaction(weaponOwner->GetFaction());
	}

	//TODO: Fix velocity vector of fired weapons



	Weapon::Fire();
}

