
#pragma once

#include "stdafx.h"

class AidsAction : public Savable {
	float actionCost;
protected:
	vec3 actionLocation;
	//Creates a spiral out from the given location
	//looking for a location invisible to the player
	vec3 findHiddenLocation(vec3 startLocation, float desiredAltitude);
public:
	//Constructing the action should start it
	AidsAction(vec3 actionLocation, float actionCost);
	virtual ~AidsAction();

	//Every subclass should have a static member GetIntensityValue()

	//retrieve the cost paid for doing this action
	float GetIntensityCost();

	//If this returns true clean up the action
	//the action has ended
	virtual bool Update() = 0;

	CLASS_DECLARATION(AidsAction)
		CLASS_MEMBER(actionLocation,ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(actionCost,ReflectionData::SAVE_FLOAT)
	END_DECLARATION
};