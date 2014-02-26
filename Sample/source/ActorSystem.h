
#pragma once

#include "stdafx.h"

#include "ContiguousList.h"

class ShaderGroup;
class Actor;

class ActorSystem {
	//All actors are updated and drawn
	ContiguousList<Actor*> allActors;
public:
	ActorSystem();
	~ActorSystem();

	//Should be called by Actor.cpp only
	//no one else call these
	void Register(Actor * toRegister);
	void Unregister(Actor * toUnregister);
	
	//Update the actors, called by base frame
	void Update(double delta, double now);

	//Draw all the actors
	void Draw(ShaderGroup * shaders);
};