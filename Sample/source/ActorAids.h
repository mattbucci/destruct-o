/**
 * Copyright (c) 2016, Nathaniel R. Lewis, Anthony Popelar, Matt Bucci, Brian Bamsch
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __ACTOR_AIDS_H__
#define __ACTOR_AIDS_H__

#include "Actor.h"
#include "ContiguousList.h"
#include "MovingAverage.h"
#include "SavableCityData.h"
#include "GameFactions.h"

#include <mutex>

class ActorAI;
class AidsAction;

//The great and powerful Artificial Intelligence Director System
class ActorAids : public Actor {
	bool spawnedNasties;

	//The id of this current update cycle
	unsigned int cycleId;

	//Requested pathing solutions
	ContiguousList<ActorAI*> pathingRequestors;
	ContiguousList<vec2> pathingRequests;

	//Intensity information
	float targetIntensity;
	//The intensity added by all the enemies deployed
	float baseIntensity;

	//Current intensity delta
	MovingAverage<float> intensityAdded;

	//The approximate current intensity
	//found by averaging all the instant intensities over the last 10 seconds
	MovingAverage<float> intensity;

	//A list of actions that can be taken by AIDS
    typedef map<float,function<AidsAction*(vec3)>> action_store;
	action_store actionList;

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

	//Given a position find the appropriate faction to assign to an actor at that position
	//Basic rules:
	//Inside a city, assign to that city
	//Outside a city, assign to the nearest city in the direction of player->point
	FactionId FindAIFactionOfPoint(vec2 point);

	//Use the AIId
	//to determine if that AI should do a heavy update, or a light update
	bool DoHeavyCycle(unsigned int aiId);

	CLASS_DECLARATION(ActorAids)
		INHERITS_FROM(Actor)
		CLASS_MEMBER(spawnedNasties,ReflectionData::SAVE_BOOL);
		CLASS_CONTAINER_MEMBER(pathingRequestors,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_HANDLE);
		CLASS_CONTAINER_MEMBER(pathingRequests,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_VEC2);

		CLASS_MEMBER(intensityAdded,ReflectionData::SAVE_INSTANCE);
		CLASS_MEMBER(intensity,ReflectionData::SAVE_INSTANCE);
		CLASS_MEMBER(intensityCalculationOffset,ReflectionData::SAVE_FLOAT);
		CLASS_CONTAINER_MEMBER(actions,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_OWNEDHANDLE);
		CLASS_CONTAINER_MEMBER(cities,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_OWNEDHANDLE);
	END_DECLARATION
};

#endif
