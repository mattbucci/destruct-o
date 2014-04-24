#pragma once
#include "AidsAction.h"

class ActorAI;

class AidsActionDeploySoldier : public AidsAction{
	ActorAI * deployed;
public:
	//This constructor is /only/ called by the loader
	AidsActionDeploySoldier();

	//Constructing the action should start it
	AidsActionDeploySoldier(vec3 actionLocation);

	//Every subclass should have a static member GetIntensityValue()
	//Intensity values in intensity units
	//100 is like a ground assault
	//10 is like one soldier
	static float GetIntensityValue();


	//If this returns true clean up the action
	//the action has ended
	bool Update() override;
	CLASS_DECLARATION(AidsActionDeploySoldier)
		INHERITS_FROM(AidsAction)
		CLASS_MEMBER(deployed,ReflectionData::SAVE_HANDLE)
	END_DECLARATION
};