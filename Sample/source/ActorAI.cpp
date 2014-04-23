#include "stdafx.h"
#include "ActorAI.h"
#include "ActorSystem.h"
#include "ActorAids.h"
#include "Universal.h"
#include "BaseFrame.h"
#include "Weapon.h"

#define TWO_PI ((float)(M_PI*2.0f))

CLASS_SAVE_CONSTRUCTOR(ActorAI);

ActorAI::ActorAI() : PhysicsActor(vec3(1.5,1.5,4),100, GameFactions::FACTION_ENEMY), __weapon(this,energyPool) {
	//Use a default weapon for now
    setModel(weapon().LookupAnimation(Weapon::ANIMATION_MODELNAME));
    playAnimation(weapon().LookupAnimation(Weapon::ANIMATION_AIM));
	//AI starts with nothing
	state = AI_SCANNING;
	//Max out the energy pool
	energyPool = 100;
}


ActorAI::~ActorAI() {
	//If you had any requests with AIDS
	//cancel them
	Actors().Aids()->CancelRequests(this);
}

//Attempt to find a close nearby enemy you can see right now
PhysicsActor * ActorAI::sightNearbyEnemy() {
	vector<PhysicsActor*> actors(Actors().GetEnemiesInRadius(Position,sightDistance(),faction));
	for (auto actor : actors) {
		//Check if you can see the physics actor
		PhysicsActor * hit;
		if (Universal::Trace(Position,glm::normalize(actor->GetPosition()-Position),NULL,&hit)) {
			if (hit == actor) {
				//We have a winner
				return hit;
			}
		}
	}
	return NULL;
}

//Turn to face that direction in the fastest possible manner
void ActorAI::applyFacingDirection(float desiredFacingDirection) {
	while (desiredFacingDirection > TWO_PI)
		desiredFacingDirection -= TWO_PI;
	while (desiredFacingDirection < 0)
		desiredFacingDirection += TWO_PI;

	//First determine how close you already are to that facing direction
	float differenceA = facingDirection - desiredFacingDirection;
	float differenceB = (TWO_PI+facingDirection) - desiredFacingDirection;
	float absDifferenceA = abs(differenceA);
	float absDifferenceB = abs(differenceB);

	//Check that error is greater than one update cycle
	if (min(absDifferenceA,absDifferenceB) < (SIMULATION_DELTA*turnSpeed())) {
		facingDirection = desiredFacingDirection;
		return;
	}
		

	//Determine which turn direction is faster
	if (absDifferenceA < absDifferenceB) {
		//Determine turn direction
		if (differenceA > 0)
			facingDirection -= SIMULATION_DELTA*turnSpeed();
		else
			facingDirection += SIMULATION_DELTA*turnSpeed();
	}
	else {
		//Determine turn direction
		if (differenceB > 0)
			facingDirection -= SIMULATION_DELTA*turnSpeed();
		else
			facingDirection += SIMULATION_DELTA*turnSpeed();
	}

	//Keep facing direction between 0 and 360
	if (facingDirection < 0)
		facingDirection += TWO_PI;
	if (facingDirection > TWO_PI)
		facingDirection -= TWO_PI;
}


bool ActorAI::Update() {
	//Check if the target enemy is still alive
	//If their actor is about to be destroyed
	//erase the reference
	if (targetEnemy->Dead())
		targetEnemy = NULL;

	//AI is a state machine
	switch (state) {
	case AI_WAITINGFORPATH:
		//While waiting for a path, if you have an enemy, go straight for them
		if (targetEnemy != NULL)
			//Set the goal
			goal = vec2(targetEnemy->GetPosition());
			//Don't break!
		else
			break;
	case AI_PATHING:
		//Check if you see an enemy nearby
		{
			PhysicsActor * seenEnemy = sightNearbyEnemy();
			if (seenEnemy != NULL) {
				//Found an enemy, target and kill
				targetEnemy = seenEnemy;
				state = AI_TARGETING_ENEMY;
				targetAcquiredAt = Game()->Now();
				break;
			}
		
			//No enemy found
			//Check how close you are
			if (glm::distance(goal,vec2(Position)) < 2) {
				//Well you're hear and there's no enemy
				//back to the drawing board
				state = AI_SCANNING;
				break;
			}

			//Face the direction you're moving
			vec2 diff = goal - vec2(Position);
			
			applyFacingDirection(atan2f(diff.y,diff.x));

			
			//If you're in the air, look like it
			bool touchingGround = false;
			if (OnGround() && Velocity.z < .2) {
				touchingGround = true;
				setAnimation(weapon().LookupAnimation(Weapon::ANIMATION_RUN));
				
			}
			else if (!animationRunning())
				setAnimation(weapon().LookupAnimation(Weapon::ANIMATION_AIM));
				

			
			//Check if you've got terrain (cliff/hill) directly in front of you
			//Check just a bit above your feet
			float feetHeight = Position.z-Size.z/2.0f;
			float checkHeight = feetHeight+.5f;
			float rayLength;
			vec3 surfaceNormal;
			vec3 traceDirection = glm::normalize(vec3(goal,checkHeight) - vec3(Position.x,Position.y,checkHeight));
			if (Game()->Voxels.RaytraceToTerrain(vec3(Position.x,Position.y,checkHeight),traceDirection,rayLength,surfaceNormal)) {
				if (rayLength < 1.5) {
					//Check the height of the given location
					//add .15 to go pas the surface and onto the voxel itself
					vec3 upcomingTerrain = vec3(Position.x,Position.y,checkHeight) + traceDirection*(rayLength+.15f);
					float upcomingHeight = Game()->Voxels.GetPositionHeight(vec2(upcomingTerrain));
					//Now check if you're too low
					if (feetHeight+.15 < upcomingHeight) {
						if (touchingGround) {
							//Ok lets jump up
							//Playing some kind of jump animation would be A+
							Velocity.z += min(15*(upcomingHeight-feetHeight),20.0f);
							//jump!
							setAnimation(weapon().LookupAnimation(Weapon::ANIMATION_JUMP));
						}
				
					}
					//Wait until you clear the edge to move forwards
					if ((!touchingGround) && (feetHeight-.15 < upcomingHeight))
						break;
						
				}
			}


			//Apply some velocity in the direction you want to move
			vec2 moveVelocity = glm::normalize(diff)*baseMovementSpeed();
			Velocity = vec3(moveVelocity,Velocity.z);

		}

		break;
	case AI_SCANNING:
		if (baseMovementSpeed() < 0) {
			//Turret
			//not implemented yet
		}
		else {
			//Not a turret find the closest enemy and try to walk to them
			targetEnemy = Actors().GetClosestEnemy(Position,faction);
			//Check if there is anything to do
			if (targetEnemy == NULL)
				break;
			//Request a pathing solution to their current position
			state = AI_WAITINGFORPATH;
			Actors().Aids()->PathingSolutionRequest(this,vec2(targetEnemy->GetPosition()));
			pathIndex = -1;
		}

		break;
	case AI_TARGETING_ENEMY:
		//BROKEN
		if (Game()->Now() - 5 > targetAcquiredAt) {
			state = AI_SCANNING;
		}
		
		
		break;
	case AI_ENGAGING_ENEMY:
		break;
	}

	//Restore some energy to your energy pool
	energyPool += SIMULATION_DELTA*100;
	if (energyPool > 100)
		energyPool = 100;

	return PhysicsActor::Update();
}

void ActorAI::PathingReady(vector<vec2> path) {
	this->path = path;
	pathIndex = 0;
	state = AI_PATHING;
}




//DEFAULTS:
//Current weapon
Weapon & ActorAI::weapon() {
	return __weapon;
}

//The time it takes to target after finding the enemy
double ActorAI::targetTime() {
	return 1.0;
}

//The movement speed of this enemy
//should be tuned to walk animation speed
float ActorAI::baseMovementSpeed() {
	return 5;
}

//How far can enemies spot each other
float ActorAI::sightDistance() {
	return 5;
}

//How many radians per second this actor can rotate
float ActorAI::turnSpeed() {
	return (float)(M_PI/2.0);
}

//Change the allegiance of this AI
void ActorAI::SetFaction(FactionId newFaction) {
	faction = newFaction;
}

void ActorAI::Draw(GLEffectProgram * effectShader)  {
	
}
