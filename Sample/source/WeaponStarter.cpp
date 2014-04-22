#include "stdafx.h"
#include "WeaponStarter.h"
#include "Universal.h"
#include "BaseFrame.h"

WeaponStarter::WeaponStarter(Actor * weaponOwner, float & chargePool) : Weapon(weaponOwner, chargePool), laser(vec4(1,.5,.1,1),.1f) {
	laser.SetTiming(.05f,1.0f,true);
}

//Whether or not the weapon should repeat firing automatically
bool WeaponStarter::RepeatFireAutomatically() {
	return false;
}

//The amount of charge it takes to fire the weapon
float WeaponStarter::WeaponChargeRequired() {
	return 0;
}

//Cooldown length for the weapon
float WeaponStarter::WeaponCooldownTime() {
	return 2;
}

//Simulate a gun shot (or laser pulse or whatever)
void WeaponStarter::Fire() {
	if (!Universal::Trace(firePoint,fireVector,&hitPos))
		hitPos = firePoint+fireVector*100.0f;
	
 	Universal::Concuss(hitPos,3,20,(PhysicsActor*)this->weaponOwner);
	laser.StartFiring();
	laser.Move(firePoint,hitPos);

	Weapon::Fire();
}
//Weapon firing animation
string WeaponStarter::FiringAnimation() {
	return "fire";
}

//Update the state of the weapon
void WeaponStarter::Update(vec3 firingVector, vec3 firingPosition) {
	Weapon::Update(firingVector,firingPosition);
}

//Draw any effects the weapon may own
void WeaponStarter::DrawWeapon(GLEffectProgram * shader, vec3 fireVector, vec3 firePoint) {
	//That's right I'm calling raytrace druing draw for the most up-to-date stuff possible
	if (!Universal::Trace(firePoint,fireVector,&hitPos))
		hitPos = firePoint+fireVector*100.0f;

	//Now move the laser to its new position and draw
	//laser.Move(firePoint,hitPos);
	laser.Draw(shader);
}