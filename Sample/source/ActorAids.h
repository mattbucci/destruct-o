
#pragma once

#include "Actor.h"
#include "ContiguousList.h"
#include "MovingAverage.h"
#include "SavableCityData.h"

#include <mutex>

class ActorAI;
class AidsAction;

//The great and powerful Artificial Intelligence Director System
class ActorAids : public Actor {
	bool spawnedNasties;

	//Requested pathing solutions
	ContiguousList<ActorAI*> pathingRequestors;
	ContiguousList<vec2> pathingRequests;
	//Intensity information
	float targetIntensity;
	float currentIntensity;
	//Current intensity delta
	MovingAverage<float> intensityAdded;
	//A list of actions that can be taken by AIDS
	map<float,function<AidsAction*(vec3)>> actionList;
	//Original intensity offset
	//used during generation of the targetIntensity
	float intensityCalculationOffset;
	//All currently executing actions
	ContiguousList<AidsAction*> actions;

	//Populate nearby cities to the player with enemies/allies
	void populateCities();

	//List of cities
	ContiguousList<SavableCityData*> cities;

	//This is my fault this is necessary
	//this is because RegisterNewCity is called from the tile gen thread
	//this is terrible
	mutex cityListMutex;
public:
	ActorAids();
	~ActorAids();

	//All of the devious logic goes here
	virtual bool Update();

	//An actor can request a pathing solution
	void PathingSolutionRequest(PhysicsActor * requestingActor, vec2 toPoint);

	//An actor (probably one pending destruction) can cancel all requests
	void CancelRequests(PhysicsActor * canclingActor);

	//Register a new city
	//this is called during tile gen
	//from the tile gen thread
	//so city access is locked by the above mutex
	void RegisterNewCity(SavableCityData * cityData);

	CLASS_DECLARATION(ActorAids)
		INHERITS_FROM(Actor)
		CLASS_MEMBER(spawnedNasties,ReflectionData::SAVE_BOOL);
		CLASS_CONTAINER_MEMBER(pathingRequestors,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_HANDLE);
		CLASS_CONTAINER_MEMBER(pathingRequests,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_VEC2);

		CLASS_MEMBER(intensityAdded,ReflectionData::SAVE_INSTANCE);
		CLASS_MEMBER(intensityCalculationOffset,ReflectionData::SAVE_FLOAT);
		CLASS_CONTAINER_MEMBER(actions,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_OWNEDHANDLE);
		CLASS_CONTAINER_MEMBER(cities,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_OWNEDHANDLE);
	END_DECLARATION
};

/*
	float targetIntensity;
	float currentIntensity;
	//Current intensity delta
	MovingAverage<float> intensityAdded;
	//A list of actions that can be taken by AIDS
	map<float,function<AidsAction*(vec3)>> actionList;
	//Original intensity offset
	//used during generation of the targetIntensity
	float intensityCalculationOffset;
	//All currently executing actions
	ContiguousList<AidsAction*> actions;

	//Populate nearby cities to the player with enemies/allies
	void populateCities();

	//List of cities
	ContiguousList<SavableCityData*> cities;
	*/