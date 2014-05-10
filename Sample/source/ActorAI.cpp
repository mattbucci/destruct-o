#include "stdafx.h"
#include "ActorAI.h"
#include "ActorSystem.h"
#include "ActorAids.h"
#include "Universal.h"
#include "BaseFrame.h"
#include "WeaponAI.h"


#include "ModelGroup.h"
#include "ModelInstance.h"
#include "Model.h"

#include <random>

#define TWO_PI ((float)(M_PI*2.0f))


CLASS_SAVE_CONSTRUCTOR(ActorAI);
SAVABLE_PAIR_IMPLEMENTATION(hatedActor);

unsigned int ActorAI::currentAIID = 0;
//How long after losing the target, the AI gives up shooting the target
static const double LostEnemyTime = 5.0;
//The distance in which an enemy notifies other enemies when it comes under attack
static const float EnemyNotifyRadius = 90.0f;
//Under this life % the AI will react to engage individual attackers faster
static const float HurtLifePercent = .5f;
//Under this life % the AI will flee if it is capable
static const float FleeLifePercent = .2f;
//When notifying other enemies of an attacker
//this factor is a % of damage to this AI, that other AI empathize with
static const float AIEmpathyFactor = .45f;
//How far away from the player a location has to be to be considered cover from the player
static const float CoverMinDistance = 50;
//At this distance, even if the enemy can see you, consider yourself to be in cover
static const float CoverMaxDistance = 200;
//How close you have to be to cover before you're considered in cover
static const float CoverArrivalDistance = 5;
//The smallest distance away from itself the AI will look for cover
static const float CoverSearchMinDistance = 5;
//The largest distance away from itself the AI will look for cover
static const float CoverSearchMaxDistance = 60;
//Check this number of random enemies before assuming a location is safe
static const int SafetyActorCheckCount = 6;
//Below this % of energy or life the AI will flee
static const float LifeFleeLevel = .2f;
static const float EnergyFleeLevel = .2f;

//Used only by the save system to create an actorai loaded
ActorAI::ActorAI() : 
	PhysicsActor(GameFactions::FACTION_HOSTILE),
	enemyPosition(10),
	stateChangesPerSec(20){
	//AI starts with nothing
	state = AI_SCANNING;
	//Max out the energy pool
	energyPool = 100;
	targetEnemy = NULL;
	actorCrashing = false;
	sawEnemyLast = 0;
	//Setup the AI ID
	aiID = currentAIID = (currentAIID+1) % 10;

	weapon = NULL;
	data = NULL;
}

//Load data into this actor class, do not apply more than once
void ActorAI::ApplyData(ActorAIData * dataToLoad) {
    this->data = dataToLoad->Copy();

    //Construct the specified weapon
    weapon = Game()->Actors.BuildWeapon(data->WeaponName,this);
    setModel(data->Model);
    playAnimation(data->AnimationLookupTable[Weapon::ANIMATION_AIM]);
    //Apply physics paramters
    Size = data->Size;
    life = maxLife = data->MaxLife;
    flying = data->Flies;
    //Rest is applied automatically
}

ActorAI::~ActorAI() {
	//If you had any requests with AIDS
	//cancel them
	Actors().Aids()->CancelRequests(this);
    //cleanup the weapon
	delete weapon;
    //cleanup the ai data
    delete data;
	//cleanup shit list
	for (auto enemy : shitList) {
		delete enemy;
		shitList.clear();
	}
}

//Check if you can see the given actor
bool  ActorAI::canSeeActor(PhysicsActor * actor) {
	//Check the actor is within your sight range
	if (glm::distance(vec2(actor->GetPosition()), vec2(Position)) > data->SightDistance)
		return false;
	//if you're close enough, do a ray trace check
	PhysicsActor * hit;
	vec3 targeter = Position+data->TargeterOffsetFromCenter;
	if (Universal::Trace(targeter,glm::normalize(actor->GetPosition()-targeter),NULL,&hit)) {
		if (hit == actor) {
			//We have a winner
			return true;
		}
	}
	return false;
}

//Whether or not this AI should flee
//true if the AI is low on health
//true if the AI is low on energy
//false if the enemy is a turret
bool ActorAI::shouldFlee() {
	return ((data->BaseMovementSpeed > 0) && (((life/maxLife) < LifeFleeLevel) || ((energyPool/energyPoolMax) < EnergyFleeLevel)));
}

//Attempt to find a close nearby enemy you can see right now
PhysicsActor * ActorAI::sightNearbyEnemy() {
	vector<PhysicsActor*> actors(Actors().GetEnemiesInRadius(Position,data->SightDistance,faction));
	for (auto actor : actors) {
		//Check if you can see the physics actor
		if (canSeeActor(actor))
			return actor;
	}
	return NULL;
}

//Turn to face that direction in the fastest possible manner
bool ActorAI::applyFacingDirection(float desiredFacingDirection) {
	while (desiredFacingDirection > TWO_PI)
		desiredFacingDirection -= TWO_PI;
	while (desiredFacingDirection < 0)
		desiredFacingDirection += TWO_PI;
	
	//First determine how close you already are to that facing direction
	float differenceA = facingDirection - desiredFacingDirection;
	float absDifferenceA = abs(differenceA);

	//Check that error is greater than one update cycle
	if (absDifferenceA < (SIMULATION_DELTA*data->TurnSpeed)) {
		facingDirection = desiredFacingDirection;
		return true;
	}
		

	//Determine which turn direction is faster
	if (differenceA > 0) {
		//Determine turn direction
		if (absDifferenceA > M_PI)
            facingDirection += SIMULATION_DELTA*data->TurnSpeed;
		else
            facingDirection -= SIMULATION_DELTA*data->TurnSpeed;
	}
	else {
		//Determine turn direction
		if (absDifferenceA > M_PI)
            facingDirection -= SIMULATION_DELTA*data->TurnSpeed;
		else
            facingDirection += SIMULATION_DELTA*data->TurnSpeed;
	}

	//Keep facing direction between 0 and 360
	if (facingDirection < 0)
		facingDirection += TWO_PI;
	if (facingDirection > TWO_PI)
		facingDirection -= TWO_PI;

	return false;
}

//Ray trace method for checking if a jump is necessary
//works well for objects of size less than 2
//tends to be more accurate than wall touch
//but fails for larger sizes
bool ActorAI::jumpCheckRayTrace(vec3 traceDirection, float feetHeight, float & jumpDifference)  {
	//Do the original ray-trace based detection
	//works well for small sized objects (soldier)
	float checkHeight = feetHeight+.5f;
	float rayLength;
	vec3 surfaceNormal;
	if (Game()->Voxels.RaytraceToTerrain(vec3(Position.x,Position.y,checkHeight),traceDirection,rayLength,surfaceNormal)) {
		if (rayLength < 1.5) {
			//Check the height of the given location
			//add .15 to go pas the surface and onto the voxel itself
			vec3 upcomingTerrain = vec3(Position.x,Position.y,checkHeight) + traceDirection*(rayLength+.15f);
			float upcomingHeight = Game()->Voxels.GetPositionHeight(vec2(upcomingTerrain));
			//Now check if you're too low
			if (feetHeight+.15 < upcomingHeight) {
				//your feet won't clear it
				jumpDifference = (upcomingHeight-feetHeight);
				return true;
			}
		}
	}
	return false;
}


//Wall touch method for checking if a jump is necessary
//works well for objects of size greater than 2
//but tends to jump in incorrect situations sometimes
bool ActorAI::jumpCheckWallTouch(vec3 traceDirection, float feetHeight, float & jumpDifference) {
	//Check if you've got terrain (cliff/hill) directly in front of you
	//Check just a bit above your feet
	if (TouchingWall()) {
		//Directly check if the physics system said you're touching a wall
		//works well for large objects (mech)
		//If so do a max based prediction of tiles in your way
		float upcomingHeight = 0.0f;
		for (float x = -(Size.x/2.0+1); x <= (Size.x/2.0+1); x++) {
			for (float y = -(Size.y/2.0+1); y <= (Size.y/2.0+1); y++) {
				vec2 pos = vec2(x,y);
				pos = glm::floor(pos);
				//check the angle between the direction you intend to go
				//and this voxel
				float angle = acos(glm::dot(pos,vec2(traceDirection))/(glm::length(traceDirection)*glm::length(pos)));
				angle = fabsf(angle);
				if (angle < M_PI/2.0f) {
					//If less than 90 degree difference
					//consider this for height
					upcomingHeight = max(upcomingHeight,Game()->Voxels.GetPositionHeight(vec2(Position)+pos));
				}
			}
		}
		//Now check if you're going to make it without jumping
		if (feetHeight+.15 < upcomingHeight) {
			//your feet won't clear it
			jumpDifference = (upcomingHeight-feetHeight);
			return true;
		}
	}
	return false;
}

//Uses the appropriate jump check to jump if necessary
//then jump the appropriate height
void ActorAI::jumpIfNecessary(vec3 moveDirection) {
				
	//The direction you intend to move
	float feetHeight = Position.z-Size.z/2.0f;

	float actorSize = max(Size.x,Size.y);

	//can't jump or climb if you're not on the ground
	if (OnGround() && Velocity.z < .2) {
		bool doJump;
		float heightDifference;
		//Check using the appropriate test for your size
		if (actorSize > 2.0f) 
			doJump = jumpCheckWallTouch(moveDirection,feetHeight,heightDifference);
		else 
			doJump = jumpCheckRayTrace(moveDirection,feetHeight,heightDifference);
		//Now do you need to jump?
		if (doJump) {
			//Ok lets jump up
			//Playing some kind of jump animation would be A+
			Velocity.z += min(15*heightDifference,20.0f)+2.0f;
			//jump!
			setAnimation(data->AnimationLookupTable[Weapon::ANIMATION_JUMP]);
		}
	}
}

bool ActorAI::Dead() {
	return PhysicsActor::Dead() || (life < 0);
}

//True if this actor is scared and is trying to make a break for it
bool ActorAI::IsFleeing() {
	return (state == AI_FLEEING) || (state == AI_TAKINGCOVER);
}

bool ActorAI::checkEnemyValid() {
	//Check you can still have an enemy
	if (!targetEnemy) {
		state = AI_SCANNING;
		return false;
	}
	
	//Check if you've seen the enemy recently
	if ((sawEnemyLast+LostEnemyTime) <= Game()->Now()) {
		targetEnemy = NULL;
		state = AI_SCANNING;
		return false;
	}

	//Check if you're not too far away
	if (glm::distance(vec2(targetEnemy->GetPosition()), vec2(Position)) > data->SightDistance*1.5f && !flying) {
		targetEnemy = NULL;
		state = AI_SCANNING;
		return false;
	}
		

	//Check if you can see the enemy now
	if (canSeeActor(targetEnemy))
		//Record that you saw them
		sawEnemyLast = Game()->Now();

	return true;
}

bool ActorAI::faceEnemy() {
	//Check if you can move the spine
	if (!checkSpineLimits()) {
		//Face the enemy you're moving to
		vec2 diff = vec2(enemyPosition.GetAverage()) - vec2(Position);
			
		return applyFacingDirection(atan2f(diff.y,diff.x));
	}
	//Else you can move the spine and don't have to manually face the enemy
	return true;
}


vec3 ActorAI::getFireVector() {
	return glm::normalize((enemyPosition.GetAverage()-vec3(0,0,.6f))-muzzlePositionA);
}


//Trigger the target acquired event
//set the state to AI_TARGETING_ENEMY
//and prepare to attack that enemy as appropriate
void ActorAI::acquireTargetEnemy(PhysicsActor * actor) {
	//Set that you've seen the enemy and setup all state appropriately
	targetEnemy = actor;
	state = AI_TARGETING_ENEMY;
	targetAcquiredAt = Game()->Now();
	sawEnemyLast = Game()->Now();
	Game()->Actors.AITargetAcquired.Fire([this](function<void(Actor *, Actor *)> observer) {
		observer(this,targetEnemy);
	});
	//Clear movement velocity
	Velocity = vec3(vec2(),Velocity.z);
}


//Move/path and jump over rocks towards the goal
bool ActorAI::pathTowardsGoal(vec2 goal) {
	//No enemy found
	//Check how close you are
	if (glm::distance(goal,vec2(Position)) < 2) {
		//Well you're here and there's no enemy
		//back to the drawing board
		targetEnemy = NULL;
		state = AI_SCANNING;
		//Clear movement velocity
		Velocity = vec3(vec2(),Velocity.z);
		return true;
	}

			
	//If you're in the air, look like it
	bool touchingGround = false;
	if (OnGround() && Velocity.z < .2) {
		touchingGround = true;
		setAnimation(data->AnimationLookupTable[Weapon::ANIMATION_RUN]);
				
	}
	else if (!animationRunning())
		setAnimation(data->AnimationLookupTable[Weapon::ANIMATION_AIM]);

	//Jump if you need to, to keep moving towards your destination
	jumpIfNecessary(glm::normalize(vec3(goal,0) - vec3(Position.x,Position.y,0)));

	return false;
}

//Move dumbly towards a goal no matter what is blocking you
void ActorAI::moveTowardsGoal(vec2 goal) {
	//Face the direction you're moving
	vec2 diff = goal - vec2(Position);
			
	applyFacingDirection(atan2f(diff.y,diff.x));
	//Apply some velocity in the direction you want to move
	vec2 moveVelocity = glm::normalize(diff)*data->BaseMovementSpeed;
	Velocity = vec3(moveVelocity,Velocity.z);
}

	//State implementations which can be overridden if necessary
void ActorAI::statePathing(bool & holdingTrigger) {
	//Only look for an enemy if you're on the ground
	//and not moving very fast in the z direction
	if (flying || (OnGround() && (fabs(Velocity.z) < .1f))) {
		//If you're going after your most hated enemy
		//go forward with single-mindedness
		//otherwise attack the first enemy you happen upon
		if ((targetEnemy != NULL) && (targetEnemy == getMostHatedEnemy())) {
			if (canSeeActor(targetEnemy)) {
				acquireTargetEnemy(targetEnemy);
			}
		}
		else {
			//Look for any enemy in your sight range and attack them
			PhysicsActor * seenEnemy = sightNearbyEnemy();
			if (seenEnemy != NULL) {
				//Found an enemy, target and kill
				acquireTargetEnemy(seenEnemy);
				return;
			}
		}
	}
	//Path towards the goal
	pathTowardsGoal(this->goal);
}
void ActorAI::stateWaitingForPath(bool & holdingTrigger) {
	//While waiting for a path, if you have an enemy, go straight for them
	if (targetEnemy != NULL)
		//Set the goal
		goal = vec2(enemyPosition.GetAverage());
}

void ActorAI::stateScanning(bool & holdingTrigger) {
	setAnimation(data->AnimationLookupTable[Weapon::ANIMATION_AIM]);

	//If you don't already have an enemy
	//find a new one now
	if (targetEnemy == NULL)
		targetEnemy = sightNearbyEnemy();
	//If you can't seen anyone right now, find an enemy you can't see
	if (targetEnemy == NULL) {
		targetEnemy = Actors().GetClosestEnemy(Position,faction);
		if ((glm::distance(vec2(targetEnemy->GetPosition()),vec2(Position)) < 5) && (!canSeeActor(targetEnemy))) {
			//You are really close to that enemy, but for some reason you can't see them
			//try backing off in a random direction
			goal = vec2(Position) + vec2(Utilities::random(-20,20),Utilities::random(-20,20));
			state = AI_WAITINGFORPATH;
		}
	}
		
	//Check if there is anything to do
	if (targetEnemy == NULL)
		return;
	if (data->BaseMovementSpeed <= 0) {
		//Check that you can see the enemy before you target them
		if (canSeeActor(targetEnemy)) {
			//You can see them make them your new enemy
			state = AI_TARGETING_ENEMY;
			targetAcquiredAt = Game()->Now();
			sawEnemyLast = Game()->Now();
			return;
		}
		//You can't see them, go back to not having a target enemy
		targetEnemy = NULL;

	}
	else {
		//Request a pathing solution to their current position
		state = AI_WAITINGFORPATH;
		Actors().Aids()->PathingSolutionRequest(this,vec2(enemyPosition.GetAverage()));
		pathIndex = -1;
	}
}

void ActorAI::stateTargeting(bool & holdingTrigger) {
	if (!checkEnemyValid())
		return;

	//Face the enemy you're targeting
	faceEnemy();

	//Check if you're done looking
	if ((targetAcquiredAt+data->TargetTime) < Game()->Now()) {
		state = AI_ENGAGING_ENEMY;
		return;
	}

	//Hang out and wait
	setAnimation(data->AnimationLookupTable[Weapon::ANIMATION_AIM]);
}
void ActorAI::stateEngaging(bool & holdingTrigger) {
	if (!checkEnemyValid())
		return;
		
	//Face the enemy you're engaging
	if (!faceEnemy())
		//If you're not facing the enemy, you can't pull the trigger
		return;


	//Pull the trigger
	holdingTrigger = true;
	weapon->Update(getFireVector(),muzzlePositionA,muzzlePositionB);
	if (weapon->HoldingTrigger(true))
		//If the weapon fired, play the fired animation
		setAnimation(data->AnimationLookupTable[Weapon::ANIMATION_FIRE]);
	else if (!animationRunning())
		//Otherwise aim at the target
		setAnimation(data->AnimationLookupTable[Weapon::ANIMATION_AIM]);
}

//Check if a location is safe for cover from a particular actor
bool ActorAI::checkIfSafeFromActor(vec3 location, PhysicsActor * actor) {
	//Check if you're far enough away from that actor
	float distFromEnemyToCover = glm::distance(actor->GetPosition(),location);
	//If you have a cover location, check that it is still safe
	if (distFromEnemyToCover < CoverMinDistance)
		//Not safe 
		return false;
	else if (distFromEnemyToCover > CoverMaxDistance)
		//safe, the enemy may be able to see you, but they're too far away (hopefully)
		return true;
	else {
		//Check if enemy has visibility of that target
		vec3 hitLocation;
		//Trace continued into infinity by default
		float traceDistance = 100000;

		if (Universal::TraceIgnoreActor(actor->GetPosition(),glm::normalize(location-actor->GetPosition()),actor,&hitLocation))
			//Now check the distance the trace went
			traceDistance = glm::distance(hitLocation,actor->GetPosition());

		//If the trace passed through the cover position, the enemy can see it
		if (traceDistance+5 < distFromEnemyToCover)
			//They can't see you in theory perhaps maybe
			return true;
		
		//Cover is not safe
		return false;
	}
}
//Check if a location is safe for cover
bool ActorAI::checkIfSafeForCover(vec3 location) {
	//Check a max of three random enemies for safety
	vector<PhysicsActor*> actors(Actors().GetEnemiesInRadius(location,CoverMaxDistance,faction));
	int check = 0;
	//Check a different three every time
	shuffle (actors.begin(), actors.end(), default_random_engine(rand()));
	for (auto actor : actors) {
		//Check if this individual actor makes the area unsafe
		if (!checkIfSafeFromActor(location,actor))
			return false;
		//Only check three actors
		if (++check >= SafetyActorCheckCount)
			return true;
	}
	//Area is safe maybe
	return true;
}

void ActorAI::stateTakingCover(bool & holdingTrigger) {
	//If you don't need to flee anymore, stop
	if (!shouldFlee()) {
		state = AI_SCANNING;
		return;
	}

	//Get the 3d representation of the cover point
	vec3 resultingPoint = vec3(coverLocation,0);
	resultingPoint.z = Game()->Voxels.GetPositionHeight(vec2(resultingPoint))+3.0f;
	//Check if the location is still safe
	if (!checkIfSafeForCover(resultingPoint)) {
		//It's not safe anymore, time to abandon ship
		state = AI_FLEEING;
	}
}

void ActorAI::stateFleeing(bool & holdingTrigger) {
	//If you don't need to flee anymore, stop
	if (!shouldFlee()) {
		state = AI_SCANNING;
		return;
	}

	//Find a location to flee towards
	//Flee away from the average location of your enemies
	vector<PhysicsActor*> actors(Actors().GetEnemiesInRadius(Position,data->SightDistance,faction));
	vec2 enemyPosition;
	//weight the enemy positions
	//so that enemies which are closer to you are more important to your decision to run
	float weightTotal = 0;
	for (auto actor : actors) {
		//actors <10 away are given max weight
		//actors >100 away are given min weight
		float weight = (glm::distance(actor->GetPosition(),Position)-10.0f)/90.0f;
		weight = min(1.0f,weight);
		weight = max(0.0f,weight);
		//Apply weight
		weightTotal += weight;
		enemyPosition += vec2(actor->GetPosition())*weight;
	}
		
	enemyPosition /= weightTotal;
	//if the enemy position is close to you flee in a specific direction
	//can't choose a random direction because then a different one would be chosen
	//over and over and the actor would never move
	vec2 fleeDirection = vec2(0,1);
	if (glm::length(enemyPosition) > .5)
		//flee away from the average enemy position
		fleeDirection = glm::normalize(vec2(Position)-enemyPosition);
	
	//Choose a point far off along the flee direction
	vec2 fleeEndPoint = fleeDirection*200.0f+vec2(Position);
	//mark it as the cover point
	coverLocation = fleeEndPoint;

	//Now try to find an /actual/ cover point
	//select a point at random in the general direction you're fleeing
	//and check to see if it's in cover or not
	//note, adding randomness in this fashion is not uniform, oh well
	vec2 randomSearchDirection = glm::normalize(fleeDirection+vec2(Utilities::random(-.25f,.25f),Utilities::random(-.25f,.25f)));
	//Select a random distance along that search direction
	float randomSearchDistance = Utilities::random(CoverSearchMinDistance,CoverSearchMaxDistance);
	//Check the resulting point and see if it would make good cover
	vec3 resultingPoint = vec3(randomSearchDirection*randomSearchDistance,0)+Position;
	resultingPoint.z = Game()->Voxels.GetPositionHeight(vec2(resultingPoint))+3.0f;
	if (checkIfSafeForCover(resultingPoint)) {
		//You found safety! Go for it buddy
		coverLocation = vec2(resultingPoint);
		state = AI_TAKINGCOVER;
	}
}
void ActorAI::stateDying(bool & holdingTrigger) {
	vulnerable = false;
	if (actorCrashing) {
		if (OnGround()) {
			//You crashed, blow up and be destroyed
			Destroy();
			Universal::Concuss(Position,data->DeathDamageRadius,data->DeathDamage,this);
			//If you have explosion particles use them
			if (data->DeathParticles.size() > 0) {
				ParticleData particlePuff = Game()->Particles.GetCached(data->DeathParticles);
				Game()->Particles.BuildParticleSystem(particlePuff, Position, .5f);
			}
			//Fire the event
			Game()->Actors.ActorVehicleCrash.Fire([this](function<void(Actor*)> observer) {
				observer(this);
			});
		}
	}
	else if (!animationRunning()) {
		state = AI_ROTTING;
        finishRotting = Game()->Now() + data->RottingTime;
	}
}
void ActorAI::stateRotting(bool & holdingTrigger) {
	if (Game()->Now() > finishRotting) {
		//Blow up if that's the sort of thing you do
		Universal::Concuss(Position,data->DeathDamageRadius,data->DeathDamage,this);
		//If you have explosion particles use them
		if (data->DeathParticles.size() > 0) {
			ParticleData particlePuff = Game()->Particles.GetCached(data->DeathParticles);
			Game()->Particles.BuildParticleSystem(particlePuff, Position, .5f);
		}
		Destroy();
	}
}

//For debug
ostream & operator<<(ostream & stream, const aiStates & state) {
	switch (state) {
	case AI_WAITINGFORPATH:
		stream << "AI_WAITINGFORPATH";
		break;
	case AI_PATHING:
		stream << "AI_PATHING";
		break;
	case AI_SCANNING:
		stream << "AI_SCANNING";
		break;
	case AI_TARGETING_ENEMY:
		stream << "AI_TARGETING_ENEMY";
		break;
	case AI_ENGAGING_ENEMY:
		stream << "AI_ENGAGING_ENEMY";
		break;
	case AI_DYING:
		stream << "AI_DYING";
		break;
	case AI_ROTTING:
		stream << "AI_ROTTING";
		break;
	}
	return stream;
}

//The smarts, ray traces and all that should go here
//planning etc.
//runs at 10hz
void ActorAI::expensiveUpdate() {
	aiStates oldState = state;

	//Update what you know of the enemies position
	if (targetEnemy != NULL)
		enemyPosition.AddSample(targetEnemy->GetPosition());
	else
		enemyPosition.Clear();


	//If you should flee, and aren't already, do so
	if (shouldFlee() && (state != AI_DYING) && (state != AI_ROTTING) && (state != AI_TAKINGCOVER))
		state = AI_FLEEING;

	bool pullingTrigger = false;

	//AI is a state machine
	switch (state) {
	case AI_WAITINGFORPATH:
		stateWaitingForPath(pullingTrigger);
	case AI_PATHING:
		//Check if you see an enemy nearby
		statePathing(pullingTrigger);
		break;
	case AI_SCANNING:
		//Don't move while you look around
		stateScanning(pullingTrigger);
		break;
	case AI_TARGETING_ENEMY:
		//Check you still have an enemy to target
		stateTargeting(pullingTrigger);
		break;
	case AI_ENGAGING_ENEMY:
		//Check you still have an enemy to engage
		stateEngaging(pullingTrigger);
		break;
	case AI_FLEEING:
		stateFleeing(pullingTrigger);
		break;
	case AI_TAKINGCOVER:
		stateTakingCover(pullingTrigger);
		break;
	case AI_DYING:
		stateDying(pullingTrigger);
		break;
	case AI_ROTTING:
		stateRotting(pullingTrigger);
		break;
	}

	//Restore some energy to your energy pool
	energyPool += SIMULATION_DELTA*100;
	if (energyPool > 100)
		energyPool = 100;

	if (!pullingTrigger)
		weapon->HoldingTrigger(false);

	if (state != oldState)
		stateChangesPerSec.AddSample(1);
	else
		stateChangesPerSec.AddSample(0);
	if (stateChangesPerSec.GetSampleCount() > 10 && stateChangesPerSec.GetAverage() > .75)
		cout << "Rapid state change warning [setbreakpointhere]\n";
		//cout << "Switched from " << oldState << " to " << state << "\n";//*/
}

//The simple things, such as moving
//or facing
//should go here
//runs at 100hz
void ActorAI::cheapUpdate() {
	//If you fly, try to maintain correct attitude, unless you're dying
	if (flying && (state != AI_DYING) && (state != AI_ROTTING)) {
		float correction = (Game()->Voxels.GetPositionHeight(vec2(Position))+data->FlyHeight) - Position.z;
		//Convert error to velocity
		correction *= .5f;
		//limit to max correction rates
		correction = max(correction,-data->AltitudeChangeRate);
        correction = min(correction, data->AltitudeChangeRate);
		//Correct directly without fooling with velocity
		//there is probably a better way
		Position.z += correction * SIMULATION_DELTA;
	}

	//AI is a state machine
	switch (state) {
	case AI_WAITINGFORPATH:
	case AI_PATHING:
		moveTowardsGoal(this->goal);
		break;
	case AI_SCANNING:
		break;
	case AI_TARGETING_ENEMY:
		//If you have an enemy, face them
		if (targetEnemy != NULL) 
			//Face the enemy you're targeting
			faceEnemy();
		break;
	case AI_ENGAGING_ENEMY:
		break;
	case AI_FLEEING:
	case AI_TAKINGCOVER:
		moveTowardsGoal(coverLocation);
		break;
	case AI_DYING:
		break;
	case AI_ROTTING:
		break;
	}
}


bool ActorAI::Update() {
	//Must be done every single update
	//before everything else

	//Check if the target enemy is still alive
	//If their actor is about to be destroyed
	//erase the reference
	if ((targetEnemy != NULL) && targetEnemy->Dead())
		targetEnemy = NULL;

	//To prevent dead enemies from ever being referenced
	//(because otherwise the save system will follow the bad references
	//during a save)
	//update the shitlist always too
	updateShitList();


	//Do expensive updates at 10hz
	if (Game()->Actors.Aids()->DoHeavyCycle(aiID))
		expensiveUpdate();
	//and do cheap updates at 100hz
	cheapUpdate();

	return PhysicsActor::Update();
}

void ActorAI::Draw(MaterialProgram * materialShader){
	if (model != NULL) {
		model->GetTransform().Translation() = vec3(Position.x,Position.y,Position.z-Size.z/2.0);
		model->GetTransform().Rotation() = glm::quat(vec3(0.5 * M_PI, 0.0, facingDirection + 0.5 * M_PI));
		model->GetTransform().Scale() = vec3(scale,scale,scale);
		//Update the weapon muzzle position

		findMuzzlePosition();
		 

		//Only recalculate if rotting isn't true
		if (state != AI_ROTTING) {
			model->Update(SIMULATION_DELTA,Game()->Now());
			//If you're facing an enemy, snap spine as best as you can
			if (targetEnemy != NULL)
				snapSpineToEnemy();
		}

		//model->Skeleton()->
		model->Draw(materialShader);

	}
}

//Overrode to prevent immediate death
void ActorAI::onDeath() {
	//Are you still alive?
	if ((state != AI_DYING) && (state != AI_ROTTING)) {
		vulnerable = false;
		actorCrashing = true;
		flying = false;
		state = AI_DYING;
		setAnimation(data->AnimationLookupTable[Weapon::ANIMATION_DEATH]);
	}
}

void ActorAI::PathingReady(vector<vec2> path) {
	this->path = path;
	pathIndex = 0;
	state = AI_PATHING;
}


//Change the allegiance of this AI
void ActorAI::SetFaction(FactionId newFaction) {
	if (faction != newFaction) {
		faction = newFaction;
		targetEnemy = NULL;
	}
}

void ActorAI::Draw(GLEffectProgram * effectShader)  {
	if (targetEnemy != NULL)
		weapon->DrawWeapon(effectShader,getFireVector(),muzzlePositionA,muzzlePositionB);
}

//Attempts to snap the user's spine to face the enemy
//returns true if successful
//Check if your spine can face the enemy right now
bool ActorAI::checkSpineLimits() {
    //Since you can move vertically pretty well
    //just check if the horizontal is very close to 0
    vec2 diff = vec2(targetEnemy->GetPosition()) - vec2(Position);

    float desired = atan2f(diff.y, diff.x);
    float horizontalDiff = abs(fmodf(desired - facingDirection, (float)M_PI));
    //Check against a tight angle check
    return horizontalDiff < data->SpineLeftRightAngleMax;
}

//Snap the model's skeleton to face the enemy
void ActorAI::snapSpineToEnemy() {
    if (data->SpineCanRotateLeftRight || data->SpineCanRotateUpDown) {
        Node * spineNode = model->Animation().Skeleton()->FindNode(data->SpineBone);
        //Apply up/down spine correction
        //TODO: Missing up/down range max
        if (data->SpineCanRotateUpDown) {
            //Take a guess at the correction in 2d only
            float distance = glm::length(targetEnemy->GetPosition() - Position);
            vec3 correctVector = glm::normalize(vec3(0, distance, targetEnemy->GetPosition().z - 1.5f - Position.z));
            //Get angle between them
            float angle = atan2(-correctVector.z, correctVector.y);

            vec3 axis = vec3(1, 0, 0);

			quat rotateFrom;
			//If this mesh is animated, rotate from the current angle
			//otherwise rotate from the unit quad
			if (model->GetModel()->Animations().size() > 1)
				rotateFrom = spineNode->LocalTransform().Rotation();
			else
				rotateFrom = quat();

            spineNode->LocalTransform().Rotation() = glm::rotate(rotateFrom, (angle+data->SpineUpDownOffset) / (float)M_PI*180.0f, axis);
            spineNode->Recalculate();
        }
        //Apply left/right spine correction
        //TODO: Missing implementation
    }
}

//Update the shit list slowly decreasing the value you hate everyone
//and removing dead enemies or ones which are too far away
//If you hate an enemy enough
//this may also switch your target to the hated enemy
void ActorAI::updateShitList() {
	bool someoneReallyHated = false;
	//Iterate through the shit list
	//and cull enemies that aren't relevant anymore
	for (auto it = shitList.begin(); it != shitList.end(); ) {
		hatedActor * hated = *it;
		//Check that the actor is not dead
		if ((hated->first->Dead()) ||
			//is not too far away
			(glm::distance(hated->first->GetPosition(),Position) > 200.0f) ||
			//is still an enemy
			(!Game()->Actors.Factions.IsEnemy(GetFaction(),hated->first->GetFaction())) ||
			//is still hated some amount
			(hated->second < .01)) {

			//If not any of those things, clean up the entry
 			delete hated;
			it = shitList.erase(it);
		}
		else {
			if (hated->second > .5) 
				someoneReallyHated = true;

			//Decrease how much you hate everyone slowly, decreases approx 40% over 10 seconds
			hated->second *= .9995;
			it++;
		}

	}
	//If you should flee, you don't care what goes next
	//worry about it later
	if (shouldFlee())
		return;

	//If you don't hate anyone particularly much, stop here
	if (!someoneReallyHated)
		return;
	//If someone is really hated and you're not currently engaging them, go for it
	PhysicsActor * mostHated = getMostHatedEnemy();
	if (targetEnemy != mostHated) {
		//Check you're still alive
		if ((state != AI_DYING) && (state != AI_ROTTING)) {
			//Switch targets immediately
			targetEnemy = mostHated;
			state  = AI_SCANNING;
			cout << "AI Switched targets due to being provoked!\n";
		}
	}
}

//Retrieve the identity of your most hated enemy
//from the shitlist
PhysicsActor * ActorAI::getMostHatedEnemy() {
	float mostHate = -1000;
	PhysicsActor * mostHated = NULL;
	//Find whichever enemy has the highest hate list
	for (auto hated : shitList) {
		if (hated->second > mostHate) {
			mostHate = hated->second;
			mostHated = hated->first;
		}
	}
	return mostHated;
}

void ActorAI::findMuzzlePosition() {
	//Node::const_flattreemap nodes;
	//model->Skeleton()->GetFlatNodeTree(nodes);

    //Find the first muzzle
    const Node * nA = model->Skeleton()->FindNode(data->MuzzleBoneA);
    _ASSERTE(nA != NULL);

    mat4 globalTransformA = model->GetTransform().TransformMatrix() * nA->TransformMatrix();

    muzzlePositionA= vec3(globalTransformA * vec4(data->MuzzleOffsetA, 1.0));
	//Check for NaN
	_ASSERTE(muzzlePositionA.x == muzzlePositionA.x);
	

    if (data->UseDualMuzzles) {
        //Find the second muzzle
        const Node * nB = model->Skeleton()->FindNode(data->MuzzleBoneB);
        _ASSERTE(nB != NULL);

        mat4 globalTransformB = model->GetTransform().TransformMatrix() * nB->TransformMatrix();

        muzzlePositionB = vec3(globalTransformB * vec4(data->MuzzleOffsetB, 1.0));
    }
}

//Add a certain number of points to this AI's shitlist in regards to a certain other AI
void ActorAI::addShitListPoints(PhysicsActor * toList, float hatePoints) {
	//Check if they're already on your shit list
	for (auto hated : shitList) {
		if (hated->first == toList) {
			//Add how much more you hate that enemy
			hated->second += hatePoints;
			return;
		}
	}
	//You must not hate them yet
	//start hating
	shitList.push_back(new hatedActor(toList,hatePoints));
}

//Damage this actor from a particular faction
//exposed in this manner to prevent implicit hiding
void ActorAI::Damage(FactionId damagingFaction, float damage) {
	PhysicsActor::Damage(damagingFaction,damage);
}

//When damage has been done to this actor
//add the damager to your shitlist
void ActorAI::Damage(PhysicsActor * damagingActor, float damage) {
	float oldLife = life;
	PhysicsActor::Damage(damagingActor,damage);
	//Mark that those mean enemies hurt you
	float damageDone = oldLife - life;
	//Add .05 because even the smallest tap is annoying
	//an
	float damageFactor = .05 + pow(damageDone/maxLife+1,2)-1;
	//If you're low life, hate them faster
	if (life/maxLife < HurtLifePercent)
		damageFactor *= 2.0;
	//Add how much you hate them to the table
	addShitListPoints(damagingActor,damageFactor);
	//Let everyone else that's on your side nearby know that you hate this guy
	//this also acts like a cry for help
	//they may come to your aid
	vector<PhysicsActor*> actors(Actors().GetActorsInRadius(Position,EnemyNotifyRadius));
	for (auto actor : actors) {
		ActorAI * otherAI = dynamic_cast<ActorAI*>(actor);
		//Can't notify them if they're not AI
		if (otherAI == NULL)
			continue;
		//Can't notify dead AI, or AI which aren't on your team
		if ((otherAI->GetFaction() != faction) || (otherAI->Dead()))
			continue;

		//Notify them
		otherAI->addShitListPoints(damagingActor,damageFactor*AIEmpathyFactor);
	}
}


//Cause the actor to have a heart attack and die on the spot
void ActorAI::Kill() {
	life = 0;
	Destroy();
}