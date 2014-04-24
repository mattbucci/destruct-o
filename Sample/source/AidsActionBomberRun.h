#pragma once
#include "AidsAction.h"

class ActorAI;

class AidsActionBomberRun : public AidsAction{
	ActorAI * deployed;
public:
	//Constructing the action should start it
	AidsActionBomberRun(vec3 actionLocation);

	//Every subclass should have a static member GetIntensityValue()
	//Intensity values in intensity units
	//100 is like a ground assault
	//10 is like one soldier
	static float GetIntensityValue();


	//If this returns true clean up the action
	//the action has ended
	bool Update() override;
};