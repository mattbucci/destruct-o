#include "stdafx.h"
#include "ActorSystem.h"
#include "Actor.h"


ActorSystem::ActorSystem() {
	lastEmpty = 0;
	highestFull = 0;
}
ActorSystem::~ActorSystem() {
	//cleanup all the actors
	//they auto-deregister, so we have to iterate over a separate list
	vector<Actor*> actors = allActors;
	for (auto actor : actors)
		delete actor;
}

//Should be called by Actor.cpp only
//no one else call these
void ActorSystem::Register(Actor * toRegister) {
	//Search for an empty space in allActors
	//accelerated by the lastEmpty property
	for (unsigned int i = lastEmpty; i < allActors.size(); i++) {
		if (allActors[i] == NULL) {
			//found an empty space
			allActors[i] = toRegister;
			//Mark the space as full
			if (i > highestFull)
				highestFull = i;
			//you know there's nothing empty below this
			lastEmpty = i+1;
			return;
		}
	}
	//Expand the actor array
	//note there is currently no way to shrink the actor away
	//so small bursts of large numbers of actors will cause array expansion
	allActors.push_back(toRegister);
	lastEmpty = allActors.size();
}
void ActorSystem::Unregister(Actor * toUnregister) {
	//Do a linear search for the actor to unregister
	//This could easily be optimized to be O(1) if actors
	//remembered their position in the actor array
	for (unsigned int i = 0; i < allActors.size(); i++) {
		if (allActors[i] == toUnregister) {
			allActors[i] = NULL;
			//remember this position as empty
			if (i < lastEmpty)
				lastEmpty = i;

			return;
		}
	}
	//Should never get here
	_ASSERTE(false);
}
	
//Update the actors, called by base frame
void ActorSystem::Update(double delta, double now) {
	unsigned int newFullSize = 0;

	//No actors mean nothing to do
	if (allActors.size() <= 0)
		return;

	//Update all the actors
	for (unsigned int i = 0; i <= highestFull; i++) {
		if (allActors[i] != NULL) {
			newFullSize = i;
			allActors[i]->Update((float)delta,(float)now);
		}
	}

	//Update the highest full size
	highestFull = newFullSize;

	//A slow (sort of) bubble sort could be used
	//to slowly compact the actor array
	//but I don't think that's necessary right now
}

//Draw all the actors
void ActorSystem::Draw(ShaderGroup * shaders) {
	unsigned int newFullSize = 0;

	//No actors mean nothing to do
	if (allActors.size() <= 0)
		return;

	//Update all the actors
	for (unsigned int i = 0; i <= highestFull; i++) {
		if (allActors[i] != NULL) {
			newFullSize = i;
			allActors[i]->Draw();
		}
	}
}