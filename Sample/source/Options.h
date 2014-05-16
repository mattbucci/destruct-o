#pragma once

#include "stdafx.h"

class Options : public Savable {
public:
	Options();
	//Options go Here
	
	//HUD Transparency
	float HUDTransparency;

	//View Distance
	//Valid range: 0 (closer) - 1 (further)
	float ViewDistance;

	//Physics accuracy
	//Valid range: 0 (least accurate) - 1 (most accurate)
	float PhysicsAccuracy;


	//Particle Quality
	//Valid range: 0 (least particles) - 1 (most particles)
	float ParticleQuality;

	//Autojump 0 is off, >0 is on
	float Autojump;

	void Print();

	CLASS_DECLARATION(Options)
		CLASS_MEMBER(HUDTransparency,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(ViewDistance,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(PhysicsAccuracy,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(ParticleQuality,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(Autojump,ReflectionData::SAVE_FLOAT)
	END_DECLARATION
};

