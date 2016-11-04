/**
 * Copyright (c) 2016, Nathaniel R. Lewis, Anthony Popelar, Matt Bucci, Brian Bamsch
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "stdafx.h"
#include "ActorAIBomber.h"
#include "ModelInstance.h"
#include "BaseFrame.h"
#include "WeaponAI.h"


CLASS_SAVE_CONSTRUCTOR(ActorAIBomber);

ActorAIBomber::ActorAIBomber() : ActorAI()  {
	flying = true;
	scale = 2;
	runStarted = false;
	turnarounddistance = 200;
}
ActorAIBomber::~ActorAIBomber() {

}

//Path until you're in range of the bomb run
void ActorAIBomber::stateEngaging(bool & holdingTrigger) {
	//check you've got an enemy
	if (targetEnemy == NULL) {
		state = AI_SCANNING;
		return;
	}

	if (!runStarted) {
		//Plan the bombing run
		//Set the run goal
		vec2 goal = vec2(targetEnemy->GetPosition());
		//Pick a point past that
		runDirection = glm::normalize(goal - vec2(Position));
		runStartPosition = vec2(Position);
		runStarted = true;
	}

	//During your run, bomb the hell out of the enemy
	vec2 diff = vec2(targetEnemy->GetPosition()) - vec2(this->GetPosition());
	if (glm::length(diff) < data->SightDistance) {
		//Make the dropped bombs face the same direction as you
		vec3 weaponFacingDirection = glm::normalize(vec3(cos(facingDirection),sin(facingDirection),0));
		weapon->Update(weaponFacingDirection,muzzlePositionA,muzzlePositionB);
		weapon->HoldingTrigger(true);
		holdingTrigger = true;
	}
}

void ActorAIBomber::cheapUpdate() {
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
		{
			if (targetEnemy == NULL) {
				state = AI_SCANNING;
				return;
			}
			//Move towards your forward direction
			Velocity = vec3(vec2(cos(this->facingDirection), sin(this->facingDirection))*data->BaseMovementSpeed, Velocity.z);

			if (glm::length(vec2(targetEnemy->GetPosition()) - vec2(this->GetPosition())) > turnarounddistance) {
				runStarted = false;
			}

			//Face the direction your run is moving
			this->applyFacingDirection(atan2(runDirection.y,runDirection.x));
		}
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
