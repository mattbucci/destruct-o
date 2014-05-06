#pragma once

#include "PhysicsActor.h"
#include "EffectLaser.h"
#include "MovingAverage.h"
#include "ActorAIData.h"

class WeaponAI;

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
	//The AI is dying dead
	AI_DYING,
	//The AI is rotting now
	AI_ROTTING,
};


class ActorAI : public PhysicsActor {
	static unsigned int currentAIID;
	//Each AI has a unique ID which decides when the expensive update is done
	//this helps even out the load of AI running
	unsigned int aiID;
protected:
	//The weapon which characterizes this AI
	//Do not change!
	WeaponAI * weapon;

	//State implementations which can be overridden if necessary
	virtual void statePathing(bool & holdingTrigger);
	virtual void stateWaitingForPath(bool & holdingTrigger);
	virtual void stateScanning(bool & holdingTrigger);
	virtual void stateTargeting(bool & holdingTrigger);
	virtual void stateEngaging(bool & holdingTrigger);
	virtual void stateDying(bool & holdingTrigger);
	virtual void stateRotting(bool & holdingTrigger);

	//Ai state
	aiStates state;
	PhysicsActor * targetEnemy;
	//How long since you prepared to fire
	double targetAcquiredAt;
	//How long since you saw that enemy
	double sawEnemyLast;
	//When the enemy should finish rotting
	double finishRotting;
	//current path
	vector<vec2> path;
	//current goal on the path
	int pathIndex;
	vec2 goal;
	//The gun's dangerous end
	vec3 muzzlePositionA;
	//If the gun has two dangerous ends
	vec3 muzzlePositionB;
	//The player position
	MovingAverage<vec3> enemyPosition;
	//If the actor is crashing (after flying)
	bool actorCrashing;

	//Retrieve the muzzle position after a draw calculate
	//and save in muzzlePositionA
	//and in muzzlePositionB if appropriate
	virtual void findMuzzlePosition();

	//Attempt to find a close nearby enemy you can see right now
	virtual PhysicsActor * sightNearbyEnemy();

	//Get the direction between you and the enemy
	vec3 getFireVector();

	//Turn to face that direction in the fastest possible manner
	//returns true when you're facing that direction
	bool applyFacingDirection(float desiredFacingDirection);


	//If the current enemy is still valid returns true
	//otherwise returns false
	bool checkEnemyValid();

	//Face the current valid enemy
	//using face direction only
	//returns true when you're facing the enemy
	bool faceEnemy();

	//Check if your spine can face the enemy right now
	virtual bool checkSpineLimits();

	//Snap the model's skeleton to face the enemy
	virtual void snapSpineToEnemy();

	//The smarts, ray traces and all that should go here
	//planning etc.
	//runs at 10hz
	virtual void expensiveUpdate();

	//The simple things, such as moving
	//or facing
	//should go here
	//runs at 100hz
	virtual void cheapUpdate();


	//AI settings
	ActorAIData * data;


	//Overrode to prevent immediate death
	virtual void onDeath() override;


public:
    //Used only by the save system to create an actorai loaded
	ActorAI();

	virtual ~ActorAI();

    //Load data into this actor class, do not apply more than once
    virtual void ApplyData(ActorAIData * dataToLoad);


	//Update the state of this AI
	virtual bool Update() override;

	//Change the allegiance of this AI
	virtual void SetFaction(FactionId newFaction);

	//Cause the actor to have a heart attack and die on the spot
	void Kill();

	//Tell other people you're dead the moment you run otu of health
	bool Dead() override;

	//When a path is granted from AIDS
	//save it
	virtual void PathingReady(vector<vec2> path);

	//override draw to prevent model from animating during STATE_ROTTING
	virtual void Draw(MaterialProgram * materialShader) override;
	
	//Draw any associated effects
	//for now draw your laser, this will be delegated in the short future
	virtual void Draw(GLEffectProgram * effectShader) override;

	CLASS_DECLARATION(ActorAI)
		INHERITS_FROM(PhysicsActor)
        CLASS_MEMBER(data, ReflectionData::SAVE_OWNEDHANDLE)
		CLASS_MEMBER(weapon,ReflectionData::SAVE_OWNEDHANDLE)
		CLASS_MEMBER(targetEnemy,ReflectionData::SAVE_HANDLE)
		CLASS_MEMBER(targetAcquiredAt,ReflectionData::SAVE_DOUBLE)
		CLASS_MEMBER(sawEnemyLast,ReflectionData::SAVE_DOUBLE);
		CLASS_MEMBER(finishRotting,ReflectionData::SAVE_DOUBLE)
		CLASS_CONTAINER_MEMBER(path,ReflectionData::SAVE_VECTOR,ReflectionData::SAVE_VEC2)
		CLASS_MEMBER(pathIndex,ReflectionData::SAVE_INT32)
		CLASS_MEMBER(goal,ReflectionData::SAVE_VEC2)
		CLASS_MEMBER(muzzlePositionA,ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(muzzlePositionB,ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(enemyPosition,ReflectionData::SAVE_INSTANCE)
		CLASS_MEMBER(actorCrashing,ReflectionData::SAVE_BOOL)
	END_DECLARATION
};

