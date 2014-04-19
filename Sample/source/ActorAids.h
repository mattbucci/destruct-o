
#pragma once

#include "Actor.h"

//The great and powerful Artificial Intelligence Director System
class ActorAids : public Actor {
	bool spawnedNasties;
public:
	ActorAids();

	//All of the devious logic goes here
	virtual bool Update();

	CLASS_DECLARATION(ActorAI)
		INHERITS_FROM(Actor)
		CLASS_MEMBER(spawnedNasties,ReflectionData::SAVE_BOOL);
	END_DECLARATION
};