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
	targetEnemy = NULL;
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
bool ActorAI::applyFacingDirection(float desiredFacingDirection) {
	while (desiredFacingDirection > TWO_PI)
		desiredFacingDirection -= TWO_PI;
	while (desiredFacingDirection < 0)
		desiredFacingDirection += TWO_PI;
	
	//First determine how close you already are to that facing direction
	float differenceA = facingDirection - desiredFacingDirection;
	float absDifferenceA = abs(differenceA);

	//Check that error is greater than one update cycle
	if (absDifferenceA < (SIMULATION_DELTA*turnSpeed())) {
		facingDirection = desiredFacingDirection;
		return true;
	}
		

	//Determine which turn direction is faster
	if (differenceA > 0) {
		//Determine turn direction
		if (absDifferenceA > M_PI)
			facingDirection += SIMULATION_DELTA*turnSpeed();
		else
			facingDirection -= SIMULATION_DELTA*turnSpeed();
	}
	else {
		//Determine turn direction
		if (absDifferenceA > M_PI)
			facingDirection -= SIMULATION_DELTA*turnSpeed();
		else
			facingDirection += SIMULATION_DELTA*turnSpeed();
	}

	//Keep facing direction between 0 and 360
	if (facingDirection < 0)
		facingDirection += TWO_PI;
	if (facingDirection > TWO_PI)
		facingDirection -= TWO_PI;

	return false;
}

bool ActorAI::Dead() {
	return PhysicsActor::Dead() || (life < 0);
}

bool ActorAI::checkEnemyValid() {
	//Check you can still see the enemy
	if (!targetEnemy) {
		state = AI_SCANNING;
		return false;
	}
		
	//Check if you can see the enemy
	PhysicsActor * hit;
	if (Universal::Trace(Position,glm::normalize(targetEnemy->GetPosition()-Position),NULL,&hit)) {
		if (hit != targetEnemy) {
			//Can't see that enemy any more
			state = AI_SCANNING;
			return false;
		}
	}

	return true;
}

bool ActorAI::faceEnemy() {
	//Face the enemy you're moving to
	vec2 diff = vec2(targetEnemy->GetPosition()) - vec2(Position);
			
	return applyFacingDirection(atan2f(diff.y,diff.x));
}

//Attempts to snap the user's spine to face the enemy
//returns true if successful
//Check if your spine can face the enemy right now
bool ActorAI::checkSpineLimits() {
	vec3 desiredFace = getFireVector();
	//Find desired angles
	//float verticalAngle = 

	//Everything is allowed
	return true;
}

float lastFloat;
int iteration = 0;

//Snap the model's skeleton to face the enemy
void ActorAI::snapSpineToEnemy() {
	//Take a guess at the correction in 2d only
	float distance = glm::length(targetEnemy->GetPosition() - Position);
	vec3 correctVector = glm::normalize(vec3(0,distance,targetEnemy->GetPosition().z - 1.5f - Position.z));
	//Get angle between them
	float angle = atan2(-correctVector.z,correctVector.y);
	lastFloat = angle;

	

	vec3 axis = vec3(1,0,0);//glm::normalize(glm::cross(desiredFace,facing));
	//float angle = fmod(OS::Now()/2,M_PI) - .5f*M_PI;
	
	quat rotation = glm::quat(glm::rotate(angle,axis));
	//Apply to the spine
	Node * spineNode = model->Animation().Skeleton()->FindNode("Bind_Spine1");


	spineNode->LocalTransform().Rotation() = glm::rotate(spineNode->LocalTransform().Rotation(),angle/M_PI*180.0f,axis);

	spineNode->Recalculate();
}


vec3 ActorAI::getFireVector() {
	return glm::normalize((targetEnemy->GetPosition()-vec3(0,0,.6f))-muzzlePosition);
}

bool ActorAI::Update() {
	//Check if the target enemy is still alive
	//If their actor is about to be destroyed
	//erase the reference
	if ((targetEnemy != NULL) && targetEnemy->Dead())
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
				Game()->Actors.AITargetAcquired.Fire([this](function<void(Actor *, Actor *)> observer) {
					observer(this,targetEnemy);
				});
				//Clear movement velocity
				Velocity = vec3(vec2(),Velocity.z);
				break;
			}
		
			//No enemy found
			//Check how close you are
			if (glm::distance(goal,vec2(Position)) < 2) {
				//Well you're hear and there's no enemy
				//back to the drawing board
				state = AI_SCANNING;
				//Clear movement velocity
				Velocity = vec3(vec2(),Velocity.z);
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
		//Don't move while you look around
		setAnimation(weapon().LookupAnimation(Weapon::ANIMATION_AIM));
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
		//Check you still have an enemy to target
		if (!checkEnemyValid())
			return false;

		//Face the enemy you're targeting
		faceEnemy();

		//Check if you're done looking
		if ((targetAcquiredAt+targetTime()) < Game()->Now()) {
			state = AI_ENGAGING_ENEMY;
			break;
		}

		//Hang out and wait
		setAnimation(weapon().LookupAnimation(Weapon::ANIMATION_AIM));
		
		
		break;
	case AI_ENGAGING_ENEMY:
		//Check you still have an enemy to engage
		if (!checkEnemyValid())
			break;
		
		//Face the enemy you're engaging
		faceEnemy();


		//Pull the trigger
		weapon().Update(getFireVector(),muzzlePosition);
		if (weapon().HoldingTrigger(true))
			//If the weapon fired, play the fired animation
			setAnimation(weapon().LookupAnimation(Weapon::ANIMATION_FIRE));
		else if (!animationRunning())
			//Otherwise aim at the target
			setAnimation(weapon().LookupAnimation(Weapon::ANIMATION_AIM));

		break;
	case AI_DYING:
		if (!animationRunning()) {
			state = AI_ROTTING;
			finishRotting = Game()->Now()+AI_ROT_TIME;
		}
		break;
	case AI_ROTTING:
		if (Game()->Now() > finishRotting) {
			Destroy();
		}
		break;
	}

	//Restore some energy to your energy pool
	energyPool += SIMULATION_DELTA*100;
	if (energyPool > 100)
		energyPool = 100;

	return PhysicsActor::Update();
}

void ActorAI::Draw(MaterialProgram * materialShader){
	if (model != NULL) {
		model->GetTransform().Translation() = vec3(Position.x,Position.y,Position.z-Size.z/2.0);
		model->GetTransform().Rotation() = glm::quat(vec3(0.5 * M_PI, 0.0, facingDirection + 0.5 * M_PI));
		//Update the weapon muzzle position

		//Find bone
		Node::const_flattreemap flatmap;
		model->Skeleton()->GetFlatNodeTree(flatmap);
		const Node * n = model->Skeleton()->FindNode("Bind_RightHand");
		mat4 globalTransform = model->GetTransform().TransformMatrix() * n->TransformMatrix();

		

		vec3 bVector(0,.5,.25);
		vec3 mVector(0,.75,.25);
		//Calculate the position along the muzzle
		beforeMuzzlePosition = vec3(globalTransform * vec4(bVector,1.0));
		muzzlePosition = vec3(globalTransform * vec4(mVector,1.0));
		 

		//Only recalculate if rotting isn't true
		if ((state != AI_ROTTING) || (state != AI_ENGAGING_ENEMY)) {
			model->Update(SIMULATION_DELTA,Game()->Now());
			//If you're facing an enemy, snap spine as best as you can
			if ((targetEnemy != NULL) && checkSpineLimits())
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
		state = AI_DYING;
		setAnimation(weapon().LookupAnimation(Weapon::ANIMATION_DEATH));
	}
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
	return 6;
}

//How far can enemies spot each other
float ActorAI::sightDistance() {
	return 20;
}

//How many radians per second this actor can rotate
float ActorAI::turnSpeed() {
	return (float)(M_PI/2.5);
}

//The angle the spine can most tilt vertically to
float ActorAI::spineVerticalAngleLimits() {
	return M_PI;
}

//The angle the spine can most tilt horizontally to
float ActorAI::spineHorizontalAngleLimits() {
	return M_PI;
}

//Change the allegiance of this AI
void ActorAI::SetFaction(FactionId newFaction) {
	faction = newFaction;
}

void ActorAI::Draw(GLEffectProgram * effectShader)  {
	if (targetEnemy != NULL)
		weapon().DrawWeapon(effectShader,getFireVector(),muzzlePosition);
}
