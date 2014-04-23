#pragma once

#include "PhysicsActor.h"
#include "EffectLaser.h"
#include "BasicAIWeapon.h"

//How long before the body disappears after death
#define AI_ROT_TIME 10

class Weapon;

class ActorAI : public PhysicsActor {
	//Temporary weapon
	BasicAIWeapon __weapon;
protected:

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
	//Tell other people you're dead the moment you run otu of health
	bool Dead() override;

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
	//This AI's charge pool
	float energyPool;
	//A part of the gun a little further up the barrel
	//a striaght line from this peice to the muzzlePosition should be the barrel
	vec3 beforeMuzzlePosition;
	//The gun's dangerous end
	vec3 muzzlePosition;



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
	bool checkSpineLimits();
	//Snap the model's skeleton to face the enemy
	virtual void snapSpineToEnemy();



	//AI settings
	//override these functions
	//to customize the AI

	//The weapon which characterizes this AI
	virtual Weapon & weapon();

	//The time it takes to target after finding the enemy
	virtual double targetTime();

	//The movement speed of this enemy
	//should be tuned to walk animation speed
	virtual float baseMovementSpeed();

	//How far can enemies spot each other
	virtual float sightDistance();

	//How many radians per second this actor can rotate
	virtual float turnSpeed();

	//The angle the spine can most tilt vertically to
	virtual float spineVerticalAngleLimits();

	//The angle the spine can most tilt horizontally to
	virtual float spineHorizontalAngleLimits();

	//Overrode to prevent immediate death
	virtual void onDeath() override;
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

	//override draw to prevent model from animating during STATE_ROTTING
	virtual void Draw(MaterialProgram * materialShader) override;
	
	//Draw any associated effects
	//for now draw your laser, this will be delegated in the short future
	virtual void Draw(GLEffectProgram * effectShader) override;

	CLASS_DECLARATION(ActorAI)
		INHERITS_FROM(PhysicsActor)
	END_DECLARATION
};