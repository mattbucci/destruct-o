#include "stdafx.h"
#include "ActorRocket.h"
#include "BaseFrame.h"
#include "WeaponAI.h"

#include "ModelGroup.h"
#include "ModelInstance.h"
#include "Model.h"

#include "ParticleSystem.h"
#include "ParticleCloud.h"
#include "BaseFrame.h"

CLASS_SAVE_CONSTRUCTOR(ActorRocket);

ActorRocket::ActorRocket() : ActorProjectile() {
	setModel("rocket");

	//Load the bomb
	bomb = Game()->Actors.BuildWeapon("rocketexplode.json",this);

	//create a particle system tail
	tail = NULL;
		
	//Set life/size appropriate for this bomb 
	maxLife = 10;
	Size = vec3(.6,.6,.6);
	flying = true;
}

ActorRocket::~ActorRocket() {
	delete bomb;
	if (tail != NULL)
		tail->Destroy();
}

bool ActorRocket::Update() {
	//If you don't have a tail, build one now
	if (tail == NULL) 
		tail = Game()->Particles.BuildParticleSystem(Game()->Particles.GetCached("rockettail.vpart"),vec3(),-1);
	//Update your tail to be behind you
	vec3 movingDirection = vec3(cos(facingDirection),sin(facingDirection),0)*-.5f+Position;
	tail->Position = movingDirection;

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


