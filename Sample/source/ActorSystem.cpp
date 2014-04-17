#include "stdafx.h"
#include "ActorSystem.h"
#include "Actor.h"
#include "ActorPlayer.h"
#include "ActorAids.h"
#include "ShaderGroup.h"
#include "MaterialProgram.h"


ActorSystem::ActorSystem(PhysicsSystem * physics) {
	this->physics = physics;
	player = BuildActor<ActorPlayer>();
	//Build AIDS too
	BuildActor<ActorAids>();
}
ActorSystem::~ActorSystem() {
	//cleanup all the actors
	for (auto actor : allActors)
		delete actor;
}

	
//Update the actors, called by base frame
void ActorSystem::Update() {

	//Update all the actors
	for (auto it = allActors.begin(); it != allActors.end(); ) 
		if ((*it)->Update()) {
			//Unregister physics actors
			if (dynamic_cast<PhysicsActor*>(*it) != NULL)
				physics->UnregisterPhysicsActor((PhysicsActor*)*it);
			//Cleanup memory
			delete *it;
			//erase from list
			it = allActors.erase(it);
		}
		else
			it++;
}

ActorPlayer * ActorSystem::Player() {
	return player;
}

//Draw all the actors
void ActorSystem::Draw(ShaderGroup * shaders) {

    // Setup the mesh shader
    MaterialProgram * modelShader = (MaterialProgram *) shaders->GetShader("model_skinned");
    modelShader->UseProgram();

	for (unsigned int i = 0; i < allActors.size(); i++) 
		allActors[i]->Draw(modelShader);
}

ContiguousList<Actor*>* ActorSystem::GetActors() {
	return &allActors;
}