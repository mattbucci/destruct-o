#pragma once
#include "AidsAction.h"

class ActorAI;

class AidsActionDeploySingle : public AidsAction{
	ActorAI * deployed;
public:
	//This constructor is /only/ called by the loader
	AidsActionDeploySingle();

	//Constructing the action should start it
	AidsActionDeploySingle(vec3 actionLocation, float actionCost, string actorToDeploy);


	//If this returns true clean up the action
	//the action has ended
	bool Update() override;
	CLASS_DECLARATION(AidsActionDeploySingle)
		INHERITS_FROM(AidsAction)
		CLASS_MEMBER(deployed,ReflectionData::SAVE_HANDLE)
	END_DECLARATION
};