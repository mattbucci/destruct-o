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

#ifndef __FIRST_PERSON_MODE_H__
#define __FIRST_PERSON_MODE_H__

#include "stdafx.h"

#include "InputEvent.h"
#include "GL2DProgram.h"

class FirstPersonMode : public Savable {
protected:
	vec3 lookVector;
	vec2 moveVector;

	//the x,y angles making up the lookVector currently
	vec2 aggregateMouseVector;
	bool firstPersonEnabled;

	// for debugging movement control
	bool debug;
	float debug_target_height;

	// Check if the trigger has been pulled
	bool triggerPulled;

	// Since the ActorPlayer polls this object per frame, GameEvents don't comply with the way the
	// rest of the object is implemented
	bool jumpRequested;

    // Does the user want to switch weapon modes
    bool weaponModeSwitchEvent;

    // Check whether the pause button has been pressed
    bool pauseRequestedEvent;

public:
	FirstPersonMode();
	~FirstPersonMode();

	//Get the normalized direction the player is currently looking
	vec3 GetLookVector();
	//Get the movement vector for the direction and magnitude
	//the player is currently moving. Magnitude is between 0 (not moving) and 1 (moving full speed)
	vec2 GetMoveVector();

	//Get the angles used to create the looking vector
	//useful for determining if the player is facing something
	//angle is in degrees
	vec2 GetAngleVector();

	// Get if the controller wants to cause the player to jump.  Checking clears the flag
	bool GetJumpRequested(bool clearFlag = true);

	//Check if the trigger is being pulled right now
	bool GetTriggerPulled();

    // Check if switch weapon is requested
    bool GetSwitchWeaponRequested(bool clearFlag = true);

    // Check if pause is requested
    bool GetPauseRequested(bool clearFlag = true);

	//debug getters
	bool GetDebug();
	float GetDebugHeight();
	void SetDebugHeight(int height);

    // Get the movement speed
    const float& GetMovementSpeed();

	//Enable or disable first person mode on platforms which require this
	void Enable(bool enableFirstPerson);

	// Read the input from the user
	virtual void ReadInput(const set<Sint64> & pressedKeys, vector<InputEvent> input);

	//In the future this will be used to draw the virtual joysticks on mobile
	virtual void Draw(double width, double height, GL2DProgram * shader);


	CLASS_DECLARATION(FirstPersonMode)
		CLASS_MEMBER(lookVector,ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(moveVector,ReflectionData::SAVE_VEC2)
		CLASS_MEMBER(debug,ReflectionData::SAVE_BOOL);
		CLASS_MEMBER(debug_target_height,ReflectionData::SAVE_FLOAT);
		CLASS_MEMBER(aggregateMouseVector,ReflectionData::SAVE_VEC2)
	END_DECLARATION
};

#endif
