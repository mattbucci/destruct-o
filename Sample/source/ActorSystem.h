
#pragma once

#include "stdafx.h"

#include "ContiguousList.h"
#include "GameFactions.h"
#include "PhysicsSystem.h"

class ShaderGroup;
class Actor;
class PhysicsActor;
class ActorPlayer;
class ActorAids;

class ActorSystem : public Savable {
	//All actors are updated and drawn
	ContiguousList<Actor*> allActors;

	//The player gets special treatment
	//that lucky bastard
	ActorPlayer * player;

	//But so does AIDS so it works out
	ActorAids * aids;

	//Keep a copy of the pointer
	//goes against the new Game() policy, but such is life
	PhysicsSystem * physics;

	//Cleanup actors in a safe manner
	void cleanActorList();
protected:
	//Overload Load so that the actor list is cleaned safely
	virtual void Load(Json::Value & parentValue, LoadData & loadData) override;
public:
	//Physics system must be constructed before actor system
	ActorSystem(PhysicsSystem * physics);
	~ActorSystem();
	
	//Construct a new actor
	template<class T>
	T * BuildActor() {
		T * actor = new T();
		allActors.push_back(actor);
		if (dynamic_cast<PhysicsActor*>(actor) != NULL)
			physics->RegisterPhysicsActor((PhysicsActor*)actor);

		return actor;	
	}

	//All factions/Teams handled here
	GameFactions Factions;

	//Update the actors, called by base frame
	void Update();

	//Get the main player
	ActorPlayer * Player();

	//Get the AIDS
	ActorAids * Aids();


	//Utility functions for actors and others
	vector<PhysicsActor*> GetActorsInRadius(vec3 center, float radius);
	vector<PhysicsActor*> GetEnemiesInRadius(vec3 center, float radius, FactionId fromFaction);
	PhysicsActor * GetClosestEnemy(vec3 from, FactionId fromFaction);
	

	//Draw all the actors
	void Draw(ShaderGroup * shaders);

	//All Actor Events
	//First actor is the actor the event is about
	GameEvent<void(Actor*)> ActorJumped;
	GameEvent<void(Actor*)> ActorLanded;
	GameEvent<void(Actor*)> ActorWalked;
	//second actor is the target acquired
	GameEvent<void(Actor*,Actor*)> AITargetAcquired;
	//second actor is the damaging actor, will be NULL if no actor associated
	//FactionId is the damaging faction
	//damage is float
	GameEvent<void(Actor*,Actor*,FactionId,float)> ActorDamaged;
	//second actor is the damaging actor, will be NULL if no actor associated
	//FactionId is the killing faction
	GameEvent<void(Actor*,Actor*,FactionId)> ActorKilled;
	//Actor Accessor
	ContiguousList<Actor*>* GetActors();

	//Actor system is essentially a list of actors
	CLASS_DECLARATION(ActorSystem)
		CLASS_CONTAINER_MEMBER(allActors,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_OWNEDHANDLE)
		CLASS_MEMBER(player,ReflectionData::SAVE_HANDLE);
		CLASS_MEMBER(Factions,ReflectionData::SAVE_INSTANCE);
	END_DECLARATION
};