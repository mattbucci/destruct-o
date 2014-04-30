#include "stdafx.h"
#include "ActorBomb.h"
#include "BaseFrame.h"
#include "WeaponAI.h"

CLASS_SAVE_CONSTRUCTOR(ActorBomb);

ActorBomb::ActorBomb() : PhysicsActor(GameFactions::FACTION_HOSTILE) {
	energyPool = 100;
	this->Velocity = vec3(0, 0, -50);
	setModel("bomb.fixme");

	//Load the bomb
	bomb = Game()->Actors.BuildWeapon("bomb.json",this);

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
		bomb->HoldingTrigger(true);
	}
	return PhysicsActor::Update();
}
