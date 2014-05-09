#include "stdafx.h"
#include "Options.h"

Options::Options() {
	//Defaults depend on the platform
	HUDTransparency = .25f;

#ifdef __MOBILE__
	ViewDistance = .25f;
	ParticleQuality = .25f;
	PhysicsAccuracy = .25f;
	Autojump = 2;
#else
	ViewDistance = .50f;
	ParticleQuality = 1.0f;
	PhysicsAccuracy = .75f;
	Autojump = 0;
#endif
}


void Options::Print() {
	cout << "Options:" << endl;
	cout << "\tView Distance: " << ViewDistance << endl;
	cout << "\tHUD Transparency: " << HUDTransparency << endl;
}