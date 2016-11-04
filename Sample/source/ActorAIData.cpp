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
