#include "stdafx.h"
#include "ActorBomb.h"

CLASS_SAVE_CONSTRUCTOR(ActorBomb);

ActorBomb::ActorBomb() : PhysicsActor(vec3(1,.5,.5),20,GameFactions::FACTION_HOSTILE), bomb(this) {
	energyPool = 100;
	setModel(bomb.LookupAnimation(Weapon::ANIMATION_MODELNAME));
}

bool ActorBomb::Update() {
	if (OnGround()) {
		//Kill yourself
		life = 0;
		Destroy();
		//Pull trigger
		bomb.Update(vec3(),Position,vec3());
		bomb.HoldingTrigger(true);
	}
	return PhysicsActor::Update();
}

void ActorBomb::SetFaction(FactionId faction) {
	this->faction = faction;
}