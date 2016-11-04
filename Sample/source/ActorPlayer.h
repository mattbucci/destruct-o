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

#ifndef __ACTOR_PLAYER_H__
#define __ACTOR_PLAYER_H__

#include "PhysicsActor.h"
#include "GameEvent.h"
#include "GameCamera.h"
#include "WeaponAI.h"

class ActorPlayer : public PhysicsActor
{
	// Change the current weapon to this weapon
	// Right now happens instantly
	void setWeapon(int weaponId);

	//delta since last walk fired
	double deltaPosition;

	// Weapon
	GameCamera       weaponCamera;
	Weapon          *currentWeapon;
	int currentWeaponId;

	// Standard player weaponry
	vector<WeaponAI*> weapons;

	//If the player fired the gun
	bool weaponFired;
	float maxEnergyPool;

	//Protect the player from actually dying
	virtual void onDeath() override;

	//When an actor is loaded
	//handles building weapons and model and such
	virtual void Load(Json::Value & parentValue, LoadData & loadData) override;

	//Weapon position
	//position of the weapon bone
	vec3 weaponBonePos;
	//Actual position of the weapon muzzle in world space
	vec3 weaponPos;
	//MVP from weapon space
	mat4 alteredMVP;
public:
	ActorPlayer();
	~ActorPlayer();

	//Retrieve's the players charge pool
	float GetCharge();
	float GetMaxCharge();

	//The player is the only actor which consumes input events
	//void ReadInput(const vector<InputEvent> & events);
	bool debug;
	float debug_target_height;


	// Update the position based off the most recent movement and direction vectors
	bool Update() override;

	// Draw the weapon
	void DrawWeapon(MaterialProgram * materialShader);


	//Get modifiers for the selected weapon
	//A damage factor of 0 indicates the weapon is locked
	WeaponModifiers * GetModifiers(int weaponId);

	//Override the normal draw
	//and do nothing
	//this prevents the Actor from trying to draw the model
	//in a way that it shouldn't
	void Draw(MaterialProgram * materialShader) override;

	//Draw the effects of said weapon
	void Draw(GLEffectProgram * shader);

	// Get a reference to the weapon camera
	GameCamera& WeaponCamera();

	CLASS_DECLARATION(ActorPlayer)
		INHERITS_FROM(PhysicsActor)
		CLASS_MEMBER(debug,ReflectionData::SAVE_BOOL);
		CLASS_MEMBER(deltaPosition,ReflectionData::SAVE_DOUBLE)
		CLASS_MEMBER(debug_target_height,ReflectionData::SAVE_FLOAT);

		//Weapon data
		CLASS_CONTAINER_MEMBER(weapons,ReflectionData::SAVE_VECTOR,ReflectionData::SAVE_OWNEDHANDLE)
		CLASS_MEMBER(currentWeaponId,ReflectionData::SAVE_INT32)
		CLASS_MEMBER(weaponFired,ReflectionData::SAVE_BOOL);
		CLASS_MEMBER(maxEnergyPool,ReflectionData::SAVE_FLOAT);
	END_DECLARATION
};

#endif
