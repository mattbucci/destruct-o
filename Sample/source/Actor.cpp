#include "stdafx.h"
#include "Actor.h"
#include "BaseFrame.h"
#include "Frames.h"

Actor::Actor() {
	valid = true;
	model = NULL;
	lastAnimationEndTime = 0;
	facingDirection = 0;
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

//Set the current model
//if modelName is invalid
//model will still be NULL
void Actor::setModel(string modelName) {
	this->modelName = modelName;
	model = Game()->Models()->NewInstance(modelName);
}

//Play an animation
//also allows animationRunning()
void Actor::playAnimation(string animationName) {
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
	lastAnimationEndTime = Game()->Now()+it->second->Length();
}


//Checks if an animation is running
//true if the animation is running
//false if the animation has looped
bool Actor::animationRunning() {
	return Game()->Now() >= lastAnimationEndTime;
}

//Draw this actor
void Actor::Draw(MaterialProgram * materialShader) {
	if (model != NULL) {
		model->GetTransform().Rotation() = glm::quat(facingDirection,vec3(0.0f, 0.0f, 1.0f));
		model->Update(SIMULATION_DELTA,Game()->Now());
		model->Draw(materialShader);
	}
}