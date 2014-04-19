#include "stdafx.h"
#include "ActorAids.h"
#include "ActorSystem.h"
#include "BaseFrame.h"
#include "ActorAI.h"

ActorAids::ActorAids() {
	spawnedNasties = false;
}

//All of the devious logic goes here
bool ActorAids::Update() {
	//Currently AIDS is pretty stupid (but won't be later)
	if (!spawnedNasties) {
		Game()->Actors.BuildActor<ActorAI>();
		Game()->Actors.BuildActor<ActorAI>();
		spawnedNasties = true;
	}
	return Actor::Update();
}