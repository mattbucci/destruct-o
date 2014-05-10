#include "stdafx.h"
#include "ActorRocket.h"
#include "BaseFrame.h"
#include "WeaponAI.h"

#include "ModelGroup.h"
#include "ModelInstance.h"
#include "Model.h"

CLASS_SAVE_CONSTRUCTOR(ActorRocket);

ActorRocket::ActorRocket() : ActorProjectile() {
	setModel("rocket");

	//Load the bomb
	bomb = Game()->Actors.BuildWeapon("rocketexplode.json",this);
		
	//Set life/size appropriate for this bomb 
	maxLife = 10;
	Size = vec3(.6,.6,.6);
	flying = true;
}

ActorRocket::~ActorRocket() {
	delete bomb;
}

bool ActorRocket::Update() {
	if (Colliding() && (CollidingWith() != firingActor)) {
		//Kill yourself
		life = 0;
		Destroy();
		//Pull trigger 
		bomb->Update(vec3(),Position,vec3());
		bomb->HoldingTrigger(true);
	}
	return ActorProjectile::Update();
}


