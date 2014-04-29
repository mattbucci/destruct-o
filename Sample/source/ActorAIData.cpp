#include "stdafx.h"
#include "ActorAIData.h"
#include "lodepng.h"

CLASS_SAVE_CONSTRUCTOR(ActorAIData);

	//Setup defaults
ActorAIData::ActorAIData() {
	ActorName = "DefaultActor";

	//Behavior
	Behavior = BEHAVIOR_GENERIC;
	Flies = false;
	TargetTime = 2.0f;
	DeathDamage = 0.0f;
	DeathDamageRadius = 0.0f;
	//Movement
	BaseMovementSpeed = 20;
	SightDistance = 20;
	TurnSpeed = 0.3141f;
	FlyHeight = 15;
	AltitudeChangeRate = 10;
	//Aesthetics
	RottingTime = 5.0f;
	Model = "";
	AnimationLookupTable = vector<string>();
	DeathParticles = "";
	//Targeting
	WaistTurnRange = 0.034906f;
	SpineCanRotateUp = false;
	SpineBone = "";
	SpineUpDownAngleMax = 20.0f;
	//Weapon
	UseDualMuzzles = false;
	MuzzleBoneA = "muzzle";
	MuzzleOffsetA = vec3();
	MuzzleBoneB = "";
	MuzzleOffsetB = vec3();
	WeaponName = "";

}

	//Save a default, used for debug purposes only
	void ActorAIData::SaveDefault() {
		ActorAIData data;
		vector<unsigned char> fileData = Savable::Serialize(&data,false);
		lodepng::save_file(fileData,"defaultai.json");
	}