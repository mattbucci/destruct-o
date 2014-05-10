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

#include "stdafx.h"
#include "FirstPersonModeMobile.h"
#include "OS.h"

// Provide constructor inheriting from base mode
FirstPersonModeMobile::FirstPersonModeMobile() : FirstPersonMode(),
	joystickNub(Rect(-75,-75,150,150),"Interface/UI/joystick.nub.png"),
	joystickOutline(Rect(-125,-125,250,250),"Interface/UI/joystick.outline.png"),
	shootIco(Rect(-50,-50,100,100),"Interface/UI/shootico.png")
{
    // Initialize finger state
    fingerLookActive = false;
    fingerJoystickActive = false;
    moveVector = vec2(0, 0);
    fingerJoystickCurrentLocation = vec2(0, 0);
	lastShootIcoUpdate= 0;
}

//
//

// Provide destructor
FirstPersonModeMobile::~FirstPersonModeMobile()
{
}

// Override standard read input to use touch controls
void FirstPersonModeMobile::ReadInput(const set<Sint64> & pressedKeys, vector<InputEvent> input)
{
    // Mouse sensitivity constants for now
	static const float mouseXSensitivity = 1.0f/6.0f;
	static const float mouseYSensitivity = 1.0f/6.0f;
    
    // Sum of all mouse deltas
	vec2 mouseDeltaSum = vec2(0, 0);
    
    // Process all events up to now
	for (auto e : input)
    {
		// Has a finger been placed on the screen?
        if(e.Event == InputEvent::MouseDown)
        {
            // Check if we should apply the joystick function (is the finger in the bounds of the joystick area
            if(!fingerJoystickActive && e.MouseX < 320.0 && e.MouseY > (screenDimensions.y - 320.0f))
            {
                // Store the finger which is controlling the joystick
                fingerJoystick = e.Key;
                
                // Store the initial location
                fingerJoystickInitialLocation = vec2(e.MouseX, e.MouseY);
                
                // Joystick is active
                fingerJoystickActive = true;
            }
            
            // If not possible for the joystick, use for looking?
            else if(!fingerLookActive)
            {
				//Check if they're over the weapon icon
				if (e.MouseX > (screenDimensions.x - 250.0f) && e.MouseY > (screenDimensions.y - 250.0f)) {
					triggerPulled = true;
					lastShootIcoUpdate = OS::Now();
				}

                // Store the finger which is controlling looking
                fingerLook = e.Key;
                
                // Store the initial position
                fingerLookPreviousLocation = vec2(e.MouseX, e.MouseY);
                
                // Looking is active
                fingerLookActive = true;
            }
            
            // Consider this finger for possible source of a jump event (caused by short tap, no motion of it though)
            possibleJumpSources.insert(pair<SDL_FingerID, double>(e.Key, OS::Now()));
        }
        
        // Has a finger been removed from the screen
        else if(e.Event == InputEvent::MouseUp)
        {
            // If the finger released was the finder controlling looking
            if(fingerLookActive && fingerLook == e.Key)
            {
                // Looking is no longer active
                fingerLookActive = false;
            }
            
            // If the finger released was the finger controlling the joystick
            else if(fingerJoystickActive && fingerJoystick == e.Key)
            {
                // Joystick is no longer active
                fingerJoystickActive = false;
                
                // Arrest motion
                moveVector = vec2(0, 0);
                fingerJoystickCurrentLocation = vec2(0, 0);
            }
            
            // Does the finger contain an entry in the possible jump sources?
            std::map<SDL_FingerID, double>::iterator it = possibleJumpSources.find(e.Key);
            if(it != possibleJumpSources.end())
            {
                // If the total time down was less than 0.15 seconds, perform a jump
                if(OS::Now() - it->second)
                {
                    // Flag for jump
                    jumpRequested = true;
                }
                
                // Remove from consideration list
                possibleJumpSources.erase(it);
            }
        }
        
        // Has a finger on the screen been moved?
        else if(e.Event == InputEvent::MouseMove)
        {
            // If the finger that moved was for the look function
            if(fingerLookActive && fingerLook == e.Key)
            {
                // Calculate the delta motion
                vec2 location = vec2(e.MouseX, e.MouseY);
                vec2 delta = location - fingerLookPreviousLocation;
                
                // Update cumulative look delta
                mouseDeltaSum -= delta;
                
				//Move the shoot icon over the location pressed
				if (triggerPulled) {
					shootIcoLocation = location;
					lastShootIcoUpdate = OS::Now();
				}

                // Update previous location
                fingerLookPreviousLocation = location;
            }
            
            // If the finger that moved was for the joystick function
            else if(fingerJoystickActive && fingerJoystick == e.Key)
            {
                // Calulate the delta position from initial location
                vec2 location = vec2(e.MouseX, e.MouseY);
                vec2 position = location - fingerJoystickInitialLocation;
                
                // Clamp values and adjust scale
                position.x = clamp<float>(position.x, -100.0, 100.0);
                position.y = clamp<float>(position.y, -100.0, 100.0);
                fingerJoystickCurrentLocation = position;
                position = position / 100.0f;
                
                // Set the move vector
                moveVector = vec2(-position.y, -position.x);
            }
            
            // Since the finger moved, remove it from jump source consideration
            auto it = possibleJumpSources.find(e.Key);
            if(it != possibleJumpSources.end())
            {
                // Remove from jump source consideration
                possibleJumpSources.erase(it);
                
                // Sadly, the reason that "if" is required is because the result of
                // map.erase(map.end()) is undefined.  Generally, it would do
                // nothing, but it is possible for something really bad to happen.
                // This scenario arises when attempting to erase something that was
                // not found
            }
        }
		else if (e.Event == InputEvent::KeyboardDown) {
			//If they pressed the back button
			//a pause is requested
			//this is how the android hardware back button is used
			if (e.Key == SDLK_AC_BACK)
				pauseRequestedEvent = true;
	
		}
	}
    
	// Now use the current mouse delta to build an aggregate
	aggregateMouseVector += mouseDeltaSum*vec2(mouseXSensitivity,mouseYSensitivity);
    
	//Limit the aggregate appropriately
	aggregateMouseVector.x = glm::mod(aggregateMouseVector.x,360.0f);
    aggregateMouseVector.y = clamp<float>(aggregateMouseVector.y, -60.0f, 75.0f);
    
	//Calculate the looking vector from the new aggregate angle vector
	//this part adapted from a previous FPS engine I wrote, so its a bit contrived
	float f = aggregateMouseVector.x/180.f*(float)M_PI;
	float t = (90.0f-aggregateMouseVector.y)/180.0f*(float)M_PI;
	lookVector.x = sin(t)*cos(f);
	lookVector.y = sin(t)*sin(f);
	lookVector.z = cos(t);
	lookVector = glm::normalize(lookVector); //IIRC this isn't necessary
    
    // If the move vector is non zero, normalize
    if(moveVector.x > 0 || moveVector.y > 0)
    {
        moveVector = glm::normalize(moveVector);
    }

	if (lastShootIcoUpdate+.1 < OS::Now())
	{
		triggerPulled = false;
		shootIcoLocation = screenDimensions - vec2(180,180);
	}
}

// Override standard drawing function
void FirstPersonModeMobile::Draw(double width, double height, GL2DProgram * shader)
{
    // Store the screen dimensions
    screenDimensions = vec2(width, height);
    
    // Enable textures for this shader
	shader->EnableTexture(true);
    	
    // Push a translation matrix
	shader->Model.PushMatrix();
    shader->Model.Translate(160, (float)height - 160, 0);
    
    // Apply the matrix
	shader->Model.Apply();
    
    // Draw the joystick outline
	joystickOutline.Draw(shader);
    
    // Pop the translation matrix
    shader->Model.PopMatrix();
	
    // Push a translation matrix
	shader->Model.PushMatrix();
    shader->Model.Translate((160) + fingerJoystickCurrentLocation.x, (float)(height - 160) + fingerJoystickCurrentLocation.y, 0);
    
    // Apply the matrix
	shader->Model.Apply();
    
    // Draw the joystick texture
	joystickNub.Draw(shader);
    
    // Pop the translation matrix
    shader->Model.PopMatrix();

    // Push a translation matrix
	shader->Model.PushMatrix();
    shader->Model.Translate(vec3(shootIcoLocation,0));
    
    // Apply the matrix
	shader->Model.Apply();
    
    // Draw the joystick texture
	shootIco.Draw(shader);
    
    // Pop the translation matrix
    shader->Model.PopMatrix();
}