
#pragma once

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

	// Since the ActorPlayer polls this object per frame, GameEvents don't comply with the way the
	// rest of the object is implemented
	bool jumpRequested;
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

	//debug getters
	bool GetDebug();
	float GetDebugHeight();
	void SetDebugHeight(int height);

	//Enable or disable first person mode on platforms which require this
	void Enable(bool enableFirstPerson);

	// Read the input from the user
	virtual void ReadInput(set<Sint64> pressedKeys, vector<InputEvent> input);

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