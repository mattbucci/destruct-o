#include "stdafx.h"
#include "ActorAids.h"
#include "ActorSystem.h"
#include "BaseFrame.h"
#include "ActorAI.h"

CLASS_SAVE_CONSTRUCTOR(ActorAids)

ActorAids::ActorAids() {
	spawnedNasties = false;
}

//All of the devious logic goes here
bool ActorAids::Update() {
	//Currently AIDS is pretty stupid (but won't be later)
	if (!spawnedNasties) {
		Game()->Actors.BuildActor<ActorAI>();
		//Game()->Actors.BuildActor<ActorAI>();
		spawnedNasties = true;
	}
	return Actor::Update();
}



//An actor can request a pathing solution
void ActorAids::PathingSolutionRequest(PhysicsActor * requestingActor, vec2 toPoint) {
	//STUB
}

//An actor (probably one pending destruction) can cancel all requests
void ActorAids::CancelRequests(PhysicsActor * canclingActor) {
	//STUB
}