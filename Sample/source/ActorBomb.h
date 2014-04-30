#pragma once

#include "PhysicsActor.h"

class WeaponAI;

class ActorBomb : public PhysicsActor {
	WeaponAI * bomb;

public:
	ActorBomb();

	virtual ~ActorBomb();
	

	virtual bool Update() override;

	CLASS_DECLARATION(ActorBomb)
		INHERITS_FROM(PhysicsActor)
		CLASS_MEMBER(bomb,ReflectionData::SAVE_OWNEDHANDLE);
	END_DECLARATION
};