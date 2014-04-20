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
	cleanActorList();
}

void ActorSystem::cleanActorList() {
	//cleanup all the actors
	//but cleanup player and aids last
	for (auto actor : allActors) {
		if ((actor != player) && (actor != aids))
			delete actor;
	}
	//Cleanup player and aids now
	delete player;
	delete aids;
}

void ActorSystem::Load(Json::Value & parentValue, LoadData & loadData) {
	//Clean actor list saftely
	cleanActorList();
	//Keep loading
	Savable::Load(parentValue,loadData);
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

//Get the AIDS
ActorAids * ActorSystem::Aids() {
	return aids;
}

//Draw all the actors
void ActorSystem::Draw(ShaderGroup * shaders) {

	// Setup the mesh shader
	MaterialProgram * modelShader = (MaterialProgram *) shaders->GetShader("model_skinned");
	modelShader->UseProgram();

	for (unsigned int i = 0; i < allActors.size(); i++) 
		allActors[i]->Draw(modelShader);
}

vector<PhysicsActor*> ActorSystem::GetActorsInRadius(vec3 center, float radius) {
	vector<PhysicsActor*> found;

	float radiusSquared = radius*radius;

	for (auto actor : allActors) {
		PhysicsActor * pactor = dynamic_cast<PhysicsActor*>(actor);
		if (pactor == NULL)
			continue;
		//Check if the actor is within the radius
		vec3 distance = pactor->GetPosition()-center;
		float distanceSquared = glm::dot(distance,distance);
		if (distanceSquared < radiusSquared)
			found.push_back(pactor);
	}

	return found;
}
vector<PhysicsActor*> ActorSystem::GetEnemiesInRadius(vec3 center, float radius, FactionId fromFaction) {
	vector<PhysicsActor*> found;

	float radiusSquared = radius*radius;

	for (auto actor : allActors) {
		PhysicsActor * pactor = dynamic_cast<PhysicsActor*>(actor);
		if (pactor == NULL)
			continue;

		//Check if the actor is an enemy
		if (!Factions.IsEnemy(fromFaction,pactor->GetFaction()))
			continue;

		//Check if the actor is within the radius
		vec3 distance = pactor->GetPosition()-center;
		float distanceSquared = glm::dot(distance,distance);
		if (distanceSquared < radiusSquared)
			found.push_back(pactor);
	}

	return found;
}

PhysicsActor * ActorSystem::GetClosestEnemy(vec3 from, FactionId fromFaction) {
	PhysicsActor* found = NULL;
	float leastDistanceSquared = 10000;

	for (auto actor : allActors) {
		PhysicsActor * pactor = dynamic_cast<PhysicsActor*>(actor);
		if (pactor == NULL)
			continue;

		//Check if the actor is an enemy
		if (!Factions.IsEnemy(fromFaction,pactor->GetFaction()))
			continue;

		//Check if the actor is the least distance away
		vec3 distance = pactor->GetPosition()-from;
		float distanceSquared = glm::dot(distance,distance);
		if (distanceSquared < leastDistanceSquared) {
			leastDistanceSquared = distanceSquared;
			found = pactor;
		}
	}

	return found;
}
ContiguousList<Actor*>* ActorSystem::GetActors() {
	return &allActors;
}