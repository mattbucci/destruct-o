#include "stdafx.h"
#include "WeaponAIEntityExplode.h"
#include "Universal.h"
#include "BaseFrame.h"

CLASS_SAVE_CONSTRUCTOR(WeaponAIEntityExplode);

WeaponAIEntityExplode::WeaponAIEntityExplode() : WeaponAI() {
}


//Simulate a gun shot (or laser pulse or whatever)
void WeaponAIEntityExplode::Fire() {
//Fire appropriate events and calculate jitter
	Weapon::Fire();
	//Apply jitter
	Weapon::updateFinalFireVectors();

	//Do first fire point

	
	weaponImpact(firePointA);
 	Universal::Concuss(firePointA,data->Radius,data->Damage,(PhysicsActor*)this->weaponOwner);
	//exploson
	if (data->WeaponHitParticles.size() > 0) {
		ParticleData particlePuff = Game()->Particles.GetCached(data->WeaponHitParticles);
		Game()->Particles.BuildParticleSystem(particlePuff, firePointA, .25f);
	}
	//muzzle flash
	if (data->MuzzleFlashParticles.size() > 0) {
		ParticleData particlePuff = Game()->Particles.GetCached(data->MuzzleFlashParticles);
		Game()->Particles.BuildParticleSystem(particlePuff, firePointA, .25f);
	}


	//Do second fire point
	if (data->DualWeapon) {
		weaponImpact(firePointA);
 		Universal::Concuss(firePointB,data->Radius,data->Damage,(PhysicsActor*)this->weaponOwner);
		//exploson
		if (data->WeaponHitParticles.size() > 0) {
			ParticleData particlePuff = Game()->Particles.GetCached(data->WeaponHitParticles);
			Game()->Particles.BuildParticleSystem(particlePuff, firePointB, .25f);
		}
		//muzzle flash
		if (data->MuzzleFlashParticles.size() > 0) {
			ParticleData particlePuff = Game()->Particles.GetCached(data->MuzzleFlashParticles);
			Game()->Particles.BuildParticleSystem(particlePuff, firePointB, .25f);
		}
	}
}
