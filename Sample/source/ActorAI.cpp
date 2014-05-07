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

#define TWO_PI ((float)(M_PI*2.0f))


CLASS_SAVE_CONSTRUCTOR(ActorAI);

unsigned int ActorAI::currentAIID = 0;
//How long after losing the target, the AI gives up shooting the target
static const double LostEnemyTime = 5.0;

//Used only by the save system to create an actorai loaded
ActorAI::ActorAI() : 
	PhysicsActor(GameFactions::FACTION_HOSTILE),
	enemyPosition(10) {
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
    maxLife = data->MaxLife;
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
}

//Attempt to find a close nearby enemy you can see right now
PhysicsActor * ActorAI::sightNearbyEnemy() {
	vector<PhysicsActor*> actors(Actors().GetEnemiesInRadius(Position,data->SightDistance,faction));
	for (auto actor : actors) {
		//Check if you can see the physics actor
		PhysicsActor * hit;
		vec3 targeter = Position+data->TargeterOffsetFromCenter;
		if (Universal::Trace(targeter,glm::normalize(actor->GetPosition()-targeter),NULL,&hit)) {
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

bool ActorAI::Dead() {
	return PhysicsActor::Dead() || (life < 0);
}

bool ActorAI::checkEnemyValid() {
	//Check you can still have an enemy
	if (!targetEnemy) {
		state = AI_SCANNING;
		return false;
	}
	
	//Check if you've seen the enemy recently
	if ((sawEnemyLast+LostEnemyTime) <= Game()->Now()) {
		state = AI_SCANNING;
		return false;
	}

	//Check if you're not too far away
	if (glm::distance(targetEnemy->GetPosition(), Position) > data->SightDistance*1.5f && !flying) {
		state = AI_SCANNING;
		return false;
	}
		

	//Check if you can see the enemy now
	PhysicsActor * hit;
	vec3 targeter = Position+data->TargeterOffsetFromCenter;
	if (Universal::Trace(targeter,glm::normalize(targetEnemy->GetPosition()-targeter),NULL,&hit)) {
		//If you can still see them record that fact
		if (hit == targetEnemy)
			sawEnemyLast = Game()->Now();
			
	}

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


	//State implementations which can be overridden if necessary
void ActorAI::statePathing(bool & holdingTrigger) {
	//Only look for an enemy if you're on the ground
	//and not moving very fast in the z direction
	if (flying || (OnGround() && (fabs(Velocity.z) < .1f))) {
		PhysicsActor * seenEnemy = sightNearbyEnemy();
		if (seenEnemy != NULL) {
			//Found an enemy, target and kill
			targetEnemy = seenEnemy;
			state = AI_TARGETING_ENEMY;
			targetAcquiredAt = Game()->Now();
			sawEnemyLast = Game()->Now();
			Game()->Actors.AITargetAcquired.Fire([this](function<void(Actor *, Actor *)> observer) {
				observer(this,targetEnemy);
			});
			//Clear movement velocity
			Velocity = vec3(vec2(),Velocity.z);
			return;
		}
	}
		
	//No enemy found
	//Check how close you are
	if (glm::distance(goal,vec2(Position)) < 2) {
		//Well you're hear and there's no enemy
		//back to the drawing board
		state = AI_SCANNING;
		//Clear movement velocity
		Velocity = vec3(vec2(),Velocity.z);
		return;
	}

			
	//If you're in the air, look like it
	bool touchingGround = false;
	if (OnGround() && Velocity.z < .2) {
		touchingGround = true;
		setAnimation(data->AnimationLookupTable[Weapon::ANIMATION_RUN]);
				
	}
	else if (!animationRunning())
		setAnimation(data->AnimationLookupTable[Weapon::ANIMATION_AIM]);
				

			
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
					setAnimation(data->AnimationLookupTable[Weapon::ANIMATION_JUMP]);
				}
				
			}
			//Wait until you clear the edge to move forwards
			if ((!touchingGround) && (feetHeight-.15 < upcomingHeight))
				return;
						
		}
	}

}
void ActorAI::stateWaitingForPath(bool & holdingTrigger) {
	//While waiting for a path, if you have an enemy, go straight for them
	if (targetEnemy != NULL)
		//Set the goal
		goal = vec2(enemyPosition.GetAverage());
}

void ActorAI::stateScanning(bool & holdingTrigger) {
	setAnimation(data->AnimationLookupTable[Weapon::ANIMATION_AIM]);

	//Not a turret find the closest enemy and try to walk to them
	targetEnemy = Actors().GetClosestEnemy(Position,faction);
	//Check if there is anything to do
	if (targetEnemy == NULL)
		return;
	if (data->BaseMovementSpeed <= 0) {
		state = AI_TARGETING_ENEMY;
		targetAcquiredAt = Game()->Now();
		sawEnemyLast = Game()->Now();
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
void ActorAI::stateDying(bool & holdingTrigger) {
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

	/*if (state != oldState)
		cout << "Switched from " << oldState << " to " << state << "\n";//*/
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
		{
			//Face the direction you're moving
			vec2 diff = goal - vec2(Position);
			
			applyFacingDirection(atan2f(diff.y,diff.x));
			//Apply some velocity in the direction you want to move
			vec2 moveVelocity = glm::normalize(diff)*data->BaseMovementSpeed;
			Velocity = vec3(moveVelocity,Velocity.z);
		}
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


//Cause the actor to have a heart attack and die on the spot
void ActorAI::Kill() {
	life = 0;
	Destroy();
}