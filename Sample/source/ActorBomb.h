#pragma once

#include "PhysicsActor.h"
#include "BombExplodeEffectWeapon.h"

class ActorBomb : public PhysicsActor {
	BombExplodeEffectWeapon bomb;

	float energyPool;
public:
	ActorBomb();

	void SetFaction(FactionId faction);

	virtual bool Update() override;
};