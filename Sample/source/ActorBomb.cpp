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
	if (OnGround()) {
		//Kill yourself
		life = 0;
		Destroy();
		//Pull trigger 
		bomb->Update(vec3(),Position,vec3());
		//bomb->HoldingTrigger(true);
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