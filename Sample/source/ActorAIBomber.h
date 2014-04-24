#pragma once

#include "ActorAI.h"
#include "EffectLaser.h"

class Weapon;

class ActorAIBomber : public ActorAI {
	bool runStarted;
	vec2 runStartPosition;
	vec2 runDirection;
	bool runFinished;
protected:
	//Bombers don't find a target and then attack it
	//they find a target and then set up a bombing run

	//Path until you're in range of the bomb run
	virtual void stateEngaging(bool & holdingTrigger);



	//Check if your spine can face the enemy right now
	virtual bool checkSpineLimits() override;

	//Snap the model's skeleton to face the enemy
	virtual void snapSpineToEnemy() override;


	//Find the muzzle position during draw
	virtual void findMuzzlePosition() override;

	//AI settings
	//override these functions
	//to customize the AI

	//The time it takes to target after finding the enemy
	virtual double targetTime() override;

	//The movement speed of this enemy
	//should be tuned to walk animation speed
	virtual float baseMovementSpeed() override;

	//How far can enemies spot each other
	virtual float sightDistance() override;

	//How many radians per second this actor can rotate
	virtual float turnSpeed() override;

	//The altitude to hover at normally
	virtual float flyHeight() override;

	//The fastest this aircraft can correct its altitude
	virtual float altitudeChangeRate() override;

public:
	ActorAIBomber();
	~ActorAIBomber();


	//Whether or not a bombing run completed
	bool BombingRunComplete();

	CLASS_DECLARATION(ActorAIBomber)
		INHERITS_FROM(ActorAI)
		CLASS_MEMBER(runStarted,ReflectionData::SAVE_BOOL);
		CLASS_MEMBER(runStartPosition,ReflectionData::SAVE_VEC2);
		CLASS_MEMBER(runDirection,ReflectionData::SAVE_VEC2);
		CLASS_MEMBER(runFinished,ReflectionData::SAVE_BOOL);
	END_DECLARATION
};
