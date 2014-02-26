#include "stdafx.h"
#include "ActorSystem.h"
#include "Actor.h"


ActorSystem::ActorSystem() {
}
ActorSystem::~ActorSystem() {
	//cleanup all the actors
	//they auto-deregister, so we have to iterate over a separate list
	ContiguousList<Actor*> actors = allActors;
	for (auto actor : actors)
		delete actor;
}

//Should be called by Actor.cpp only
//no one else call these
void ActorSystem::Register(Actor * toRegister) {
	allActors.push_back(toRegister);
}
void ActorSystem::Unregister(Actor * toUnregister) {
	allActors.erase(toUnregister);
}
	
//Update the actors, called by base frame
void ActorSystem::Update(double delta, double now) {

	//Update all the actors
	for (unsigned int i = 0; i < allActors.size(); i++) 
		allActors[i]->Update((float)delta,(float)now);
}

//Draw all the actors
void ActorSystem::Draw(ShaderGroup * shaders) {
	for (unsigned int i = 0; i < allActors.size(); i++) 
		allActors[i]->Draw();
}