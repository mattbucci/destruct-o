#pragma once

#include "PhysicsActor.h"
#include "EffectLaser.h"

class ActorAI : public PhysicsActor {
	EffectLaser laser;
protected:

	enum animation {
		ANIMATION_IDLE,
		ANIMATION_AIM,
		ANIMATION_WALK,
		ANIMATION_JUMP,
		ANIMATION_DEATH,
		ANIMATION_INAIR,
	};


	enum aiStates {
		//Moving towards an enemy
		//Following a given path
		AI_PATHING,
		//Waiting for a new path
		AI_WAITINGFORPATH,
		//When the AI is looking for an opponent they enter this state
		AI_SCANNING,
		//When an AI initially spots an enemy they can't directly engage
		//they must target for a bit first
		AI_TARGETING_ENEMY,
		//When you're done targeting, kill'm
		AI_ENGAGING_ENEMY,
	};

	//Ai state
	aiStates state;
	PhysicsActor * targetEnemy;
	//How long since you prepared to fire
	double targetAcquiredAt;
	//How long since you saw that enemy
	double sawEnemyLast;
	//current path
	vector<vec2> path;
	//current goal on the path
	int pathIndex;
	vec2 goal;
	

	//Attempt to find a close nearby enemy you can see right now
	virtual PhysicsActor * sightNearbyEnemy();

	//Turn to face that direction in the fastest possible manner
	void applyFacingDirection(float desiredFacingDirection);

	//AI settings
	//override these functions
	//to customize the AI

	//Lookup the animation name based off the constant
	//in the future this will be full virtual
	virtual const string & lookupAnim(animation animationId);

	//The time it takes to target after finding the enemy
	virtual double targetTime();

	//The movement speed of this enemy
	//should be tuned to walk animation speed
	virtual float baseMovementSpeed();

	//How far can enemies spot each other
	virtual float sightDistance();

	//How many radians per second this actor can rotate
	virtual float turnSpeed();

public:
	ActorAI();
	~ActorAI();

	//Update the state of this AI
	virtual bool Update() override;

	//Change the allegiance of this AI
	virtual void SetFaction(FactionId newFaction);


	//When a path is granted from AIDS
	//save it
	virtual void PathingReady(vector<vec2> path);
	
	//Draw any associated effects
	//for now draw your laser, this will be delegated in the short future
	virtual void Draw(GLEffectProgram * effectShader) override;

	CLASS_DECLARATION(ActorAI)
		INHERITS_FROM(PhysicsActor)
	END_DECLARATION
};