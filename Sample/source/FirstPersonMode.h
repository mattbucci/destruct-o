
#pragma once

#include "stdafx.h"

#include "InputEvent.h"

class FirstPersonMode {
	vec3 lookVector;
	vec2 moveVector;
	//the x,y angles making up the lookVector currently
	vec2 aggregateMouseVector;
	bool firstPersonEnabled;
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

	//Enable or disable first person mode on platforms which require this
	void Enable(bool enableFirstPerson);

	//Update the looking direction based off input events
	void UpdateLookingDirection(set<int> pressedKeys, vector<InputEvent> input);

	//In the future this will be used to draw the virtual joysticks on mobile
	void Draw();
};