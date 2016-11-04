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

#ifndef __FIRSTPERSONMODEMOBILE_H__
#define __FIRSTPERSONMODEMOBILE_H__

// Precompiled header
#include "stdafx.h"

// Need base first person controller
#include "FirstPersonMode.h"

// Need 2D vertex groups and shaders
#include "GLModel.h"
#include "GL2DVertexGroup.h"

// Timing functions
#include "OS.h"

#include "TexturedRect.h"

// Subclass of FirstPersonMode which handles the first person controller on mobile devices.
// Will draw a virtual joystick for motion, accepts tapping and dragging on the screen
// anywhere but the joystick as looking.
class FirstPersonModeMobile : public FirstPersonMode
{
protected:
	bool sprinting;
    // Current finger controlling looking
    SDL_FingerID fingerLook;
    bool         fingerLookActive;
    vec2         fingerLookPreviousLocation;

    // Current finger controlling joystick
    SDL_FingerID fingerJoystick;
    bool         fingerJoystickActive;
    vec2         fingerJoystickCurrentLocation;
    vec2         fingerJoystickInitialLocation;

    // Possible causes of a jump event
    map<SDL_FingerID, double> possibleJumpSources;

    // Previous know screen dimensions
    vec2         screenDimensions;

	TexturedRect joystickOutline;
	TexturedRect joystickNub;
	TexturedRect shootIco;
	vec2 shootIcoLocation;
	double lastShootIcoUpdate;

public:
	FirstPersonModeMobile();
	~FirstPersonModeMobile();

	// Process input
	virtual void ReadInput(const set<Sint64> & pressedKeys, vector<InputEvent> input) override;

	// Draw the virtual joysticks
	virtual void Draw(double width, double height, GL2DProgram * shader);
};

#endif
