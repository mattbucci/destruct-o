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
    MaxLife = 100.0f;
	//Movement
	BaseMovementSpeed = 20;
	SightDistance = 20;
	TurnSpeed = 0.3141f;
	FlyHeight = 15;
	AltitudeChangeRate = 10;
    Size = vec3(1, 1, 1);
	//Aesthetics
	RottingTime = 5.0f;
	Model = "";
	AnimationLookupTable = vector<string>();
	DeathParticles = "";
	//Targeting
	TargeterOffsetFromCenter = vec3();
	SpineCanRotateUpDown = false;
	SpineUpDownAngleMax = 20.0f;
    SpineCanRotateLeftRight = false;
    SpineLeftRightAngleMax = 0.034906f;
    SpineBone = "";
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

//Clone the actor ai data
ActorAIData * ActorAIData::Copy() {
    return new ActorAIData(*this);
}