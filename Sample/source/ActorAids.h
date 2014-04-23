
#pragma once

#include "Actor.h"
#include "ContiguousList.h"
#include "MovingAverage.h"

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
public:
	ActorAids();

	//All of the devious logic goes here
	virtual bool Update();

	//An actor can request a pathing solution
	void PathingSolutionRequest(PhysicsActor * requestingActor, vec2 toPoint);

	//An actor (probably one pending destruction) can cancel all requests
	void CancelRequests(PhysicsActor * canclingActor);

	CLASS_DECLARATION(ActorAI)
		INHERITS_FROM(Actor)
		CLASS_MEMBER(spawnedNasties,ReflectionData::SAVE_BOOL);
		CLASS_CONTAINER_MEMBER(pathingRequestors,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_HANDLE);
		CLASS_CONTAINER_MEMBER(pathingRequests,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_VEC2);
	END_DECLARATION
};