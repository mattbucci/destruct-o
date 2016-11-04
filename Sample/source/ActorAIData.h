/**
 * Copyright (c) 2016, Nathaniel R. Lewis, Anthony Popelar, Matt Bucci, Brian Bamsch
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __ACTOR_AI_DATA_H__
#define __ACTOR_AI_DATA_H__

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

	//The offset from center
	//of the targeting device (head,eye,etc.)
	//if this position can see the player
	//the whole entity can
	vec3 TargeterOffsetFromCenter;

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

	//An offset for the calculated spine angle (radians)
	float SpineUpDownOffset;

	//The amount the spine can rotate up/down in radians
	float SpineUpDownAngleMax;

    //Whether or not the user can rotate his spine left,right
    bool SpineCanRotateLeftRight;

	//An offset for the calculated spine angle (radians)
	float SpineLeftRightOffset;

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

		CLASS_MEMBER(TargeterOffsetFromCenter,ReflectionData::SAVE_VEC3);
		CLASS_MEMBER(SpineCanRotateUpDown,ReflectionData::SAVE_BOOL)
		CLASS_MEMBER(SpineUpDownOffset,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(SpineUpDownAngleMax,ReflectionData::SAVE_FLOAT)
        CLASS_MEMBER(SpineCanRotateLeftRight, ReflectionData::SAVE_BOOL)
		CLASS_MEMBER(SpineLeftRightAngleMax, ReflectionData::SAVE_FLOAT)
        CLASS_MEMBER(SpineLeftRightOffset, ReflectionData::SAVE_FLOAT)
        CLASS_MEMBER(SpineBone, ReflectionData::SAVE_STRING)

		CLASS_MEMBER(UseDualMuzzles,ReflectionData::SAVE_BOOL)
		CLASS_MEMBER(MuzzleBoneA,ReflectionData::SAVE_STRING)
		CLASS_MEMBER(MuzzleOffsetA,ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(MuzzleBoneB,ReflectionData::SAVE_STRING)
		CLASS_MEMBER(MuzzleOffsetB,ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(WeaponName, ReflectionData::SAVE_STRING)
	END_DECLARATION
};

#endif
