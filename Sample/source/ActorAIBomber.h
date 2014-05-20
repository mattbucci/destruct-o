#pragma once

#include "ActorAI.h"
#include "EffectLaser.h"

class Weapon;

class ActorAIBomber : public ActorAI {
	bool runStarted;
	vec2 runStartPosition;
	vec2 runDirection;
	int turnarounddistance;
protected:
	//Bombers don't find a target and then attack it
	//they find a target and then set up a bombing run

	//Path until you're in range of the bomb run
	virtual void stateEngaging(bool & holdingTrigger);


public:
	ActorAIBomber();
	~ActorAIBomber();

	//The simple things, such as moving
	//or facing
	//should go here
	//runs at 100hz
	virtual void cheapUpdate();


	//Whether or not a bombing run completed
	bool BombingRunComplete();

	CLASS_DECLARATION(ActorAIBomber)
		INHERITS_FROM(ActorAI)
		CLASS_MEMBER(runStarted,ReflectionData::SAVE_BOOL);
		CLASS_MEMBER(runStartPosition,ReflectionData::SAVE_VEC2);
		CLASS_MEMBER(runDirection,ReflectionData::SAVE_VEC2);
	END_DECLARATION
};
