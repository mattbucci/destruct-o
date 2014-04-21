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

// Provide constructor inheriting from base mode
FirstPersonModeMobile::FirstPersonModeMobile() : FirstPersonMode()
{
    // Initialize finger state
    fingerLookActive = false;
    fingerJoystickActive = false;
    moveVector = vec2(0, 0);
    fingerJoystickCurrentLocation = vec2(0, 0);
    
    // Setup the verticies for the joystick texture
    joystickVertices = new GL2DVertexGroup(GL_TRIANGLE_STRIP, 4);
    joystickOutlineVertices = new GL2DVertexGroup(GL_TRIANGLE_STRIP, 4);
    
    // Push verticies and texcoords for joystick outline
    const float joystick_x = 150.f;
    const float joystick_y = 150.f;
    joystickVertices->svat(0,vec2(-joystick_x/2, -joystick_y/2));
    joystickVertices->svat(1,vec2(-joystick_x/2, joystick_y/2));
    joystickVertices->svat(2,vec2(joystick_x/2, -joystick_y/2));
    joystickVertices->svat(3,vec2(joystick_x/2, joystick_y/2));
    joystickVertices->sxat(0,vec2(0, 0));
    joystickVertices->sxat(1,vec2(0, 1));
    joystickVertices->sxat(2,vec2(1, 0));
    joystickVertices->sxat(3,vec2(1, 1));
    
    // Push vertices and texcoords for joystick outline
    const float joystickOutline_x = 250.f;
    const float joystickOutline_y = 250.f;
    joystickOutlineVertices->svat(0,vec2(-joystickOutline_x/2, -joystickOutline_y/2));
    joystickOutlineVertices->svat(1,vec2(-joystickOutline_x/2, joystickOutline_y/2));
    joystickOutlineVertices->svat(2,vec2(joystickOutline_x/2, -joystickOutline_y/2));
    joystickOutlineVertices->svat(3,vec2(joystickOutline_x/2, joystickOutline_y/2));
    joystickOutlineVertices->sxat(0,vec2(0, 0));
    joystickOutlineVertices->sxat(1,vec2(0, 1));
    joystickOutlineVertices->sxat(2,vec2(1, 0));
    joystickOutlineVertices->sxat(3,vec2(1, 1));
    
    // Load textures from disk
	unsigned int textureWidth, textureHeight;
	vector<unsigned char> image;
	unsigned error = lodepng::decode(image,textureWidth,textureHeight,"Interface/UI/joystick.nub.png");
    if(error)
    {
        SDL_Log("Joystick Nub Texture - Lodepng error (%u) - %s", error, lodepng_error_text(error));
    }
    
    // Generate the texture object for the joystick
    glGenTextures(1, &joystickTexture);
	glBindTexture(GL_TEXTURE_2D, joystickTexture);
    
	//Is actually stored in  GL_BGRA but that can't be loaded in opengl es apparently
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0] );
    
    // Texture parameters (filters, non-power of two, etc.)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // Load the joystick outline image
    image.clear();
    error = lodepng::decode(image,textureWidth,textureHeight,"Interface/UI/joystick.outline.png");
    if(error)
    {
        SDL_Log("Joystick Outline Texture - Lodepng error (%u) - %s", error, lodepng_error_text(error));
    }
    
    // Generate the texture object for the joystick
    glGenTextures(1, &joystickOutlineTexture);
	glBindTexture(GL_TEXTURE_2D, joystickOutlineTexture);
    
	//Is actually stored in  GL_BGRA but that can't be loaded in opengl es apparently
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0] );
    
    // Texture parameters (filters, non-power of two, etc.)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

// Provide destructor
FirstPersonModeMobile::~FirstPersonModeMobile()
{
    // Delete the joystick textures
    glDeleteTextures(1, &joystickTexture);
    glDeleteTextures(1, &joystickOutlineTexture);

    // Delete vertex group
    delete joystickVertices;
    delete joystickOutlineVertices;
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
            std::map<SDL_FingerID, double>::iterator it = possibleJumpSources.find(e.Key);
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
}

// Override standard drawing function
void FirstPersonModeMobile::Draw(double width, double height, GL2DProgram * shader)
{
    // Store the screen dimensions
    screenDimensions = vec2(width, height);
    
    // Enable textures for this shader
	shader->EnableTexture(true);
    
    // Active the first texture unit
	glActiveTexture (GL_TEXTURE0);
    
    // Bind the outline texture
	glBindTexture(GL_TEXTURE_2D, joystickOutlineTexture);
	
    // Push a translation matrix
	shader->Model.PushMatrix();
    shader->Model.Translate(160, (float)height - 160, 0);
    
    // Apply the matrix
	shader->Model.Apply();
    
    // Draw the joystick outline
	joystickOutlineVertices->Draw(shader);
    
    // Pop the translation matrix
    shader->Model.PopMatrix();
    
    // Bind the joystick texture
	glBindTexture(GL_TEXTURE_2D, joystickTexture);
	
    // Push a translation matrix
	shader->Model.PushMatrix();
    shader->Model.Translate((160) + fingerJoystickCurrentLocation.x, (float)(height - 160) + fingerJoystickCurrentLocation.y, 0);
    
    // Apply the matrix
	shader->Model.Apply();
    
    // Draw the joystick texture
	joystickVertices->Draw(shader);
    
    // Pop the translation matrix
    shader->Model.PopMatrix();
}