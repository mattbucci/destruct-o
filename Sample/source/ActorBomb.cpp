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
#include "ActorBomb.h"
#include "BaseFrame.h"
#include "WeaponAI.h"

#include "ModelGroup.h"
#include "ModelInstance.h"
#include "Model.h"

CLASS_SAVE_CONSTRUCTOR(ActorBomb);

ActorBomb::ActorBomb() {
	energyPool = 100;
	this->Velocity = vec3(0, 0, -50);
	setModel("bomb");

	//Load the bomb
	bomb = Game()->Actors.BuildWeapon("bombexplode.json",this);

	//Set life/size appropriate for this bomb
	maxLife = 20;
	Size = vec3(1,.5,.5);
}

ActorBomb::~ActorBomb() {
	delete bomb;
}

bool ActorBomb::Update() {
	if (Colliding() && (CollidingWith() != firingActor)) {
		//Kill yourself
		life = 0;
		Destroy();
		//Pull trigger
		bomb->Update(vec3(),Position,vec3());
		bomb->HoldingTrigger(true);
	}
	return PhysicsActor::Update();
}


	//Draw this actor
//And any associated model
//override draw to fix rotation issue with this model
void ActorBomb::Draw(MaterialProgram * materialShader) {
	if (model != NULL) {
		//Fix facing direction for the bomb model here
		model->GetTransform().Rotation() = glm::quat(vec3(0.5 * M_PI, 0.0, facingDirection + M_PI));
		model->GetTransform().Scale() = vec3(scale,scale,scale);
		model->GetTransform().Translation() = vec3(Position.x,Position.y,Position.z-Size.z/2.0);
		model->Update(SIMULATION_DELTA,Game()->Now());
		model->Draw(materialShader);
	}
}
