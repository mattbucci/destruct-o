#pragma once

#include "ActorAI.h"
#include "EffectLaser.h"

//How long before the body disappears after death
#define AI_ROT_TIME 10

class Weapon;

class ActorAIMosquito : public ActorAI {
protected:

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
	ActorAIMosquito();
	~ActorAIMosquito();

	CLASS_DECLARATION(ActorAIMosquito)
		INHERITS_FROM(ActorAI)
	END_DECLARATION
};