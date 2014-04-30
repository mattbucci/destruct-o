#include "stdafx.h"
#include "Universal.h"
#include "BaseFrame.h"
#include "WeaponAILaser.h"
#include "LaserWeaponData.h"

CLASS_SAVE_CONSTRUCTOR(WeaponAILaser);

WeaponAILaser::WeaponAILaser() {
}

//Simulate a gun shot (or laser pulse or whatever)
void WeaponAILaser::Fire() {
	//Fire appropriate events and calculate jitter
	Weapon::Fire();
	//Apply jitter
	Weapon::updateFinalFireVectors();

	//Do first fire point
	if (!Universal::TraceIgnoreActor(firePointA,finalFireVectorA,(PhysicsActor*)weaponOwner,&hitPosA))
		hitPosA = firePointA+finalFireVectorA*100.0f;
	
	weaponImpact(hitPosA);
 	Universal::Concuss(hitPosA,laserData->Radius,laserData->Damage,(PhysicsActor*)this->weaponOwner);

	
	laserA.StartFiring();
	laserA.Move(firePointB,hitPosB);


	//Do second fire point
	if (laserData->DualWeapon) {
		if (!Universal::TraceIgnoreActor(firePointB,finalFireVectorB,(PhysicsActor*)weaponOwner,&hitPosB))
			hitPosB = firePointB+finalFireVectorB*100.0f;
	
		weaponImpact(hitPosB);
 		Universal::Concuss(hitPosB,laserData->Radius,laserData->Damage,(PhysicsActor*)this->weaponOwner);

		laserB.StartFiring();
		laserB.Move(firePointB,hitPosB);
	}



}

//Update the state of the weapon
void WeaponAILaser::Update(vec3 firingVector, vec3 firePointA, vec3 FirePointB) {
	Weapon::Update(firingVector, firePointA, FirePointB);
}

//Draw any effects the weapon may own
void WeaponAILaser::DrawWeapon(GLEffectProgram * shader, vec3 fireVector, vec3 firePointA, vec3 firePointB) {
	if (laserData->LaserFollowMuzzle) {
		//That's right I'm calling raytrace during draw for the most up-to-date stuff possible
		if (!Universal::Trace(firePointA,finalFireVectorA,&hitPosA))
			hitPosA = firePointA+finalFireVectorA*100.0f;
	}
	//Now move the laser to its new position and draw
	laserA.Move(firePointA,hitPosA);
	laserA.Draw(shader);

	if (laserData->DualWeapon) {
		if (laserData->LaserFollowMuzzle) {
			//That's right I'm calling raytrace during draw for the most up-to-date stuff possible
			if (!Universal::Trace(firePointB,finalFireVectorB,&hitPosB))
				hitPosB = firePointB+finalFireVectorB*100.0f;
		}
		//Now move the laser to its new position and draw
		laserB.Move(firePointB,hitPosB);
		laserB.Draw(shader);
	}

}


//Apply the given weapon data to this weapon
void WeaponAILaser::ApplyData(WeaponData * weaponData) {
	WeaponAI::ApplyData(weaponData);
	laserData = dynamic_cast<LaserWeaponData*>(data);
	//Better be laser data
	_ASSERTE(laserData != NULL);

	//Setup lasers
	laserA.SetLaserColor(laserData->LaserColor);
	laserA.SetTiming(laserData->LaserWarmUpTime,laserData->LaserCooldownTime,true);

	laserB.SetLaserColor(laserData->LaserColor);
	laserB.SetTiming(laserData->LaserWarmUpTime,laserData->LaserCooldownTime,true);

}