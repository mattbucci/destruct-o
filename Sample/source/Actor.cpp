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
#include "Actor.h"
#include "BaseFrame.h"
#include "Frames.h"

#include "ModelGroup.h"
#include "ModelInstance.h"
#include "Model.h"

Actor::Actor() {
	valid = true;
	model = NULL;
	lastAnimationEndTime = 0;
	facingDirection = 0;
	scale = 1;
	tiltDirection = 0;
}
Actor::~Actor() {
	delete model;
}

//Safely erase this actor during the next update cycle
void Actor::Destroy() {
	valid = false;
}


//Update this actor with the delta and current time
bool Actor::Update() {
	return !valid;
}

bool Actor::Dead() {
	return !valid;
}

//Set the current model
//if modelName is invalid
//model will still be NULL
void Actor::setModel(const string & modelName) {
	this->modelName = modelName;
    //Delete the old model
    delete model;
	model = Game()->Models()->NewInstance(modelName);
    //Clear the current animation
    lastPlayedAnimation = "";
    lastAnimationEndTime = Game()->Now();
}

//Play an animation
//also allows animationRunning()
void Actor::playAnimation(const string & animationName) {
	if (!model)
		return;
	//Retrieve the animation length
	auto & anim = model->GetModel()->Animations();
	auto it = anim.find(animationName);
	//Check animation exists
	if (it == anim.end())
		return;
	//Set as the current animation
	model->PlayAnimation(animationName);
	lastPlayedAnimation = animationName;
	lastAnimationEndTime = Game()->Now()+it->second->Length();
}


void Actor::setAnimation(const string & animationName) {
	if (!model)
		return;
	if (lastPlayedAnimation != animationName)
		playAnimation(animationName);
}

//Checks if an animation is running
//true if the animation is running
//false if the animation has looped
bool Actor::animationRunning() {
	return Game()->Now() < lastAnimationEndTime;
}

//Draw this actor
void Actor::Draw(MaterialProgram * materialShader) {
	//facingDirection = fmod(OS::Now(),360);
	if (model != NULL) {
		//Normalize the quaternion after they're combined
		//or terrible terrible things happen (trust me)
		model->GetTransform().Rotation() = glm::quat(vec3(0.5 * M_PI + tiltDirection, 0.0, facingDirection + 0.5 * M_PI));
		model->GetTransform().Scale() = vec3(scale,scale,scale);
		model->Update(SIMULATION_DELTA,Game()->Now());
		model->Draw(materialShader);
	}
}

void Actor::Draw(GLEffectProgram * effectShader) {

}

//Get the position of this physics actor
vec3 Actor::GetPosition() {
	return position;
}


//When an actor is loaded
//handles rebuilding the model instance
void Actor::Load(Json::Value & parentValue, LoadData & loadData) {
	//Reload all class data
	Savable::Load(parentValue,loadData);
	//If you had a model, rebuild it
	if (modelName.size() > 0) {
		setModel(modelName);
	}
}

ActorSystem & Actor::Actors() {
	return Game()->Actors;
}
