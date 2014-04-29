
#pragma once

#include "stdafx.h"
typedef enum : int  {
	BEHAVIOR_GENERIC,
	BEHAVIOR_BOMBER,
} ActorBehaviors;

class ActorAIData : public Savable {
public:
	//Setup defaults
	ActorAIData();

	//Save a default, used for debug purposes only
	static void SaveDefault();

	//A unique name identifying this actor
	string ActorName;

	//The behavior of this actor
	//of type ActorBehaviors
	ActorBehaviors Behavior;

	//Whether or not this AI flies
	bool Flies;

    //The max health of this AI
    float MaxLife;

	//The time it takes to target after finding the enemy
	double TargetTime;

	//The movement speed of this enemy
	//should be tuned to walk animation speed
	float BaseMovementSpeed;

	//How far can enemies spot each other
	float SightDistance;

	//How many radians per second this actor can rotate
	float TurnSpeed;

	//The altitude to hover at normally
	float FlyHeight;

	//The fastest this aircraft can correct its altitude
	float AltitudeChangeRate;

    //The size of this actor's physics box
    vec3 Size;

	//The amount of time the body of this actor "rots" before despawning
	float RottingTime;

	//If this actor has two muzzles
	//use both of them
	//otherwise only uses A
	bool UseDualMuzzles;

	//The muzzle bone of the gun
	string MuzzleBoneA;

	//The offset from the above bone which weapon fire should come from
	vec3 MuzzleOffsetA;

	//The muzzle bone of the gun
	string MuzzleBoneB;

	//The offset from the above bone which weapon fire should come from
	vec3 MuzzleOffsetB;

	//Whether or not the user can rotate his spine up,down
	bool SpineCanRotateUpDown;

	//The amount the spine can rotate up/down in radians
	float SpineUpDownAngleMax;

    //Whether or not the user can rotate his spine left,right
    bool SpineCanRotateLeftRight;

    //The amount the spine can rotate left/right in radians
    //even if SpineCanRotateLeftRight
    //is false, this should be about 1 degree
    //so the target can be a little off, and the ai can still fire
    float SpineLeftRightAngleMax;

    //The bone to use to rotate the spine
    string SpineBone;

	//The name of the weapon employed by this actor
	string WeaponName;

	//The model of this actor
	string Model;

	//The animation lookup table
	//for this actor
	vector<string> AnimationLookupTable;

	//This damage is done to surrounding actors on death
	float DeathDamage;

	//The radius in which the damage is done
	float DeathDamageRadius;

	//The particle effect attached to death
	string DeathParticles;
	

    //Clone the actor ai data
    virtual ActorAIData * Copy();

	//All actor ai data
	CLASS_DECLARATION(ActorAIData)
		CLASS_MEMBER(ActorName,ReflectionData::SAVE_STRING)

		CLASS_MEMBER(Behavior,ReflectionData::SAVE_INT32)
		CLASS_MEMBER(Flies,ReflectionData::SAVE_BOOL)
		CLASS_MEMBER(TargetTime,ReflectionData::SAVE_DOUBLE)
		CLASS_MEMBER(DeathDamage, ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(DeathDamageRadius,ReflectionData::SAVE_FLOAT)
        CLASS_MEMBER(MaxLife, ReflectionData::SAVE_FLOAT)
		
		CLASS_MEMBER(BaseMovementSpeed,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(SightDistance,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(TurnSpeed,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(FlyHeight,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(AltitudeChangeRate,ReflectionData::SAVE_FLOAT)
        CLASS_MEMBER(Size, ReflectionData::SAVE_VEC3)

		CLASS_MEMBER(RottingTime,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(Model, ReflectionData::SAVE_STRING)
		CLASS_CONTAINER_MEMBER(AnimationLookupTable,ReflectionData::SAVE_VECTOR,ReflectionData::SAVE_STRING)
		CLASS_MEMBER(DeathParticles, ReflectionData::SAVE_STRING);

		CLASS_MEMBER(SpineCanRotateUpDown,ReflectionData::SAVE_BOOL)
		CLASS_MEMBER(SpineUpDownAngleMax,ReflectionData::SAVE_FLOAT)
        CLASS_MEMBER(SpineCanRotateLeftRight, ReflectionData::SAVE_BOOL)
        CLASS_MEMBER(SpineLeftRightAngleMax, ReflectionData::SAVE_FLOAT)
        CLASS_MEMBER(SpineBone, ReflectionData::SAVE_STRING)

		CLASS_MEMBER(UseDualMuzzles,ReflectionData::SAVE_BOOL)
		CLASS_MEMBER(MuzzleBoneA,ReflectionData::SAVE_STRING)
		CLASS_MEMBER(MuzzleOffsetA,ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(MuzzleBoneA,ReflectionData::SAVE_STRING)
		CLASS_MEMBER(MuzzleOffsetB,ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(WeaponName, ReflectionData::SAVE_STRING)
	END_DECLARATION
};