/* 
 *  Copyright 2014 Nathaniel Lewis, Matthew Bucci, Anthony Popelar, Brian Bamsch
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
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