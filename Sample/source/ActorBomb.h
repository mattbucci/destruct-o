#pragma once

#include "PhysicsActor.h"
#include "BombExplodeEffectWeapon.h"

class ActorBomb : public PhysicsActor {
	BombExplodeEffectWeapon bomb;

public:
	ActorBomb();

	void SetFaction(FactionId faction);

	virtual bool Update() override;

	CLASS_DECLARATION(ActorBomb)
		INHERITS_FROM(PhysicsActor)
		CLASS_MEMBER(bomb,ReflectionData::SAVE_INSTANCE);
	END_DECLARATION
};