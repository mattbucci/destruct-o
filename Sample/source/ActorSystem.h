
#pragma once

#include "stdafx.h"

#include "ContiguousList.h"
#include "GameFactions.h"
#include "AIWeaponCache.h"
#include "AIDataCache.h"
#include "PhysicsSystem.h"

class ShaderGroup;
class Actor;
class PhysicsActor;
class ActorPlayer;
class ActorAids;
class Weapon;
class PhysicsSystem;
class ActorAI;
class WeaponAI;

class ActorSystem : public Savable {
	//All actors are updated and drawn
	ContiguousList<Actor*> allActors;

	//All actors scheduled for destruction
	ContiguousList<Actor*> deadActors1;
	ContiguousList<Actor*> deadActors2;
	//Newly born actors waiting to be added to the actor list
	ContiguousList<Actor*> newlyBornActors;

	//Clean an actor list without touching the important actors
	void cleanList(ContiguousList<Actor*> & actorList);

	//Flip these back and forth like double buffers
	bool useDeadActors1;

	//The player gets special treatment
	//that lucky bastard
	ActorPlayer * player;

	//But so does AIDS so it works out
	ActorAids * aids;

	//Keep a copy of the pointer
	//goes against the new Game() policy, but such is life
	PhysicsSystem * physics;

    //Retrieve ai actor data, loaded during Load()
    AIDataCache AIProfiles;

    //Retrieve weapon data, loaded during Load()
    AIWeaponCache Weapons;

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
	//do not call during physics events (sorry)
	template<class T>
	T * BuildActor(vec3 position = vec3()) {
		T * actor = new T();
		newlyBornActors.push_back(actor);
		//Note: if BuildActor is called during a physics event
		//shit goes bad
		if (dynamic_cast<PhysicsActor*>(actor) != NULL)
			physics->RegisterPhysicsActor((PhysicsActor*)actor);

		actor->position = position;
		return actor;	
	}

    //Construct a new AI at position from file
    //do not call during physics events (sorry)
    ActorAI * BuildAI(vec3 position, string filename);
    
    //Build an AI weapon from the filename of the weapon
    WeaponAI * BuildWeapon(string filename);

	//All factions/Teams handled here
	GameFactions Factions;

	//Cache ai data
	void Load();

	//Update the actors, called by base frame
	void Update();

	//Get the main player
	ActorPlayer * Player();

	//Get the AIDS
	ActorAids * Aids();

	//Do AOE damage
	//to actors
	//also flings actors
	void DoAOEDamage(vec3 at, float radius, float damage, PhysicsActor * damager);

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
	//Weapon related things
	//The actor, the weapon fired
	GameEvent<void(Actor*,Weapon*)> ActorFiredWeapon;
	//The actor, the weapon fired, the impact location
	GameEvent<void(Actor*,Weapon*,vec3)> ActorWeaponImpact;
	//Happens with the player tries to fire without enough energy
	//(the player is the only one stupid enough to try this)
	GameEvent<void(Actor*,Weapon*)> ActorFiredWhileEmpty;
	//If an air unit crashes this event fires
	GameEvent<void(Actor*)> ActorVehicleCrash;

	//Actor Accessor
	ContiguousList<Actor*>* GetActors();

	//Actor system is essentially a list of actors
	CLASS_DECLARATION(ActorSystem)
		CLASS_CONTAINER_MEMBER(allActors,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_OWNEDHANDLE)
		CLASS_CONTAINER_MEMBER(deadActors1,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_OWNEDHANDLE)
		CLASS_CONTAINER_MEMBER(deadActors2,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_OWNEDHANDLE)
		CLASS_CONTAINER_MEMBER(newlyBornActors,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_OWNEDHANDLE)
		CLASS_MEMBER(useDeadActors1,ReflectionData::SAVE_BOOL)
		CLASS_MEMBER(player,ReflectionData::SAVE_HANDLE)
		CLASS_MEMBER(aids,ReflectionData::SAVE_HANDLE)
		CLASS_MEMBER(physics,ReflectionData::SAVE_HANDLE)
		CLASS_MEMBER(Factions,ReflectionData::SAVE_INSTANCE)
	END_DECLARATION
};