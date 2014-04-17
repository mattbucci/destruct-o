
#pragma once

#include "stdafx.h"

#include "ContiguousList.h"

class ShaderGroup;
class Actor;

class ActorSystem : public Savable {
	//All actors are updated and drawn
	ContiguousList<Actor*> allActors;

	//Should be called by Actor.cpp only
	//no one else call these
	void Register(Actor * toRegister);
	void Unregister(Actor * toUnregister);

	friend class Actor;
public:
	ActorSystem();
	~ActorSystem();
	
	//Update the actors, called by base frame
	void Update();

	//Draw all the actors
	void Draw(ShaderGroup * shaders);

	//Actor system is essentially a list of actors
	CLASS_DECLARATION(ActorSystem)
		CLASS_CONTAINER_MEMBER(allActors,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_OWNEDHANDLEAR)
	END_DECLARATION
};