#pragma once

#include "PhysicsActor.h"

class ActorAI : public PhysicsActor {
public:
	ActorAI();
	~ActorAI();

	CLASS_DECLARATION(ActorAI)
		INHERITS_FROM(PhysicsActor)
	END_DECLARATION
};