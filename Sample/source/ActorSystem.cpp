#include "stdafx.h"
#include "ActorSystem.h"
#include "Actor.h"
#include "ActorAI.h"
#include "ActorPlayer.h"
#include "ActorAids.h"
#include "ShaderGroup.h"
#include "MaterialProgram.h"
#include "GLEffectProgram.h"
#include "WeaponAI.h"

ActorSystem::ActorSystem(PhysicsSystem * physics) {
	this->physics = physics;
	player = BuildActor<ActorPlayer>();
	//Build AIDS too
	aids = BuildActor<ActorAids>();
}
ActorSystem::~ActorSystem() {
	cleanActorList();
}

	//Clean an actor list without touching the important actors
void ActorSystem::cleanList(ContiguousList<Actor*> & actorList) {
	//cleanup all the actors
	//but cleanup player and aids last
	for (auto actor : actorList) {
		if ((actor != player) && (actor != aids))
			delete actor;
	}
	actorList.clear();
}

void ActorSystem::cleanActorList() {
	//cleanup all the actors
	//but cleanup player and aids last
	cleanList(newlyBornActors);
	cleanList(deadActors1);
	cleanList(deadActors2);
	cleanList(allActors);

	//Cleanup player and aids now
	delete player;
	delete aids;
}

//Builds an actor by name
//this only works with actors specially designed for it to work
//AI will NOT work through this call
//but simple physics actors should
Actor * ActorSystem::BuildActorFromName(string name) {
    //Abuse the save system to construct an AI with the given name
    Actor * actor = dynamic_cast<Actor*>(ReflectionStore::Data().RetrieveClassInstance(name));
	//The actor should be valid
	_ASSERTE(actor != NULL);
	//Note: if BuildActor is called during a physics event
	//shit goes bad
	if (dynamic_cast<PhysicsActor*>(actor) != NULL)
		physics->RegisterPhysicsActor((PhysicsActor*)actor);

	return actor;
}

//Construct a new AI at position from file
//do not call during physics events (sorry)
ActorAI * ActorSystem::BuildAI(vec3 position, string filename) {
    //Use these macros to find the string name
    //of the class within the same system
    //this is used to retrieve an instance of the class, by name, through the reflection data
    //maintained by the save system
    static const string aiClassLookup[2] = {
        //BEHAVIOR_GENERIC
        "ActorAI",
        //BEHAVIOR_BOMBER
        "ActorAIBomber"
    };
    //Retrieve the ai data
    ActorAIData * aiData = AIProfiles.GetCached(filename);

    //Abuse the save system to construct an AI with the given name
    ActorAI * actor = dynamic_cast<ActorAI*>(ReflectionStore::Data().RetrieveClassInstance(aiClassLookup[aiData->Behavior]));
	//The actor should be valid
	_ASSERTE(actor != NULL);
    newlyBornActors.push_back(actor);

    //Note: if BuildActor is called during a physics event
    //shit goes bad
    physics->RegisterPhysicsActor((PhysicsActor*)actor);

    //set the actor position
    actor->position = position;

    //Apply the actor data to the new actor
    actor->ApplyData(aiData);

    return actor;
}

//Build an AI weapon from the filename of the weapon
WeaponAI * ActorSystem::BuildWeapon(string filename, PhysicsActor * owner) {
    //Use these macros to find the string name
    //of the class within the same system
    //this is used to retrieve an instance of the class, by name, through the reflection data
    //maintained by the save system
    static const string weaponClassLookup[3] = {
        //WEAPONBEHAVIOR_LASER
        "WeaponAILaser",
        //WEAPONBEHAVIOR_ENTITYFIRE
        "WeaponAIEntityFire",
        //WEAPONBEHAVIOR_ENTITYEXPLODE
        "WeaponAIEntityExplode"
    };
    //Retrieve the ai data
    WeaponData * weaponData = Weapons.GetCached(filename);

    //Abuse the save system to construct an AI with the given name
    WeaponAI * weapon = dynamic_cast<WeaponAI*>(ReflectionStore::Data().RetrieveClassInstance(weaponClassLookup[weaponData->Behavior]));
	//The weapon should be valid
	_ASSERTE(weapon != NULL);
    //Apply the weapon data to the new actor
    weapon->ApplyData(weaponData);
	weapon->SetOwner(owner);

    return weapon;
}

void ActorSystem::Load(Json::Value & parentValue, LoadData & loadData) {
	//Clean actor list safely
	cleanActorList();
	//Keep loading
	Savable::Load(parentValue,loadData);
}

//Cache ai data
void ActorSystem::Load() {
	AIProfiles.Load();
	Weapons.Load();
}

//Update the actors, called by base frame
void ActorSystem::Update() {
	//Swap these like a double buffer
	useDeadActors1 = !useDeadActors1;
	//The previously died list is the previous recentlyDead list
	//and the previous previously died list is reused as the new recentlyDead list
	ContiguousList<Actor*> & recentlyDied = useDeadActors1 ? deadActors1 : deadActors2;
	ContiguousList<Actor*> & previouslyDied = useDeadActors1 ? deadActors2 : deadActors1;

	//Place newly born actors in the actor array
	for (auto newActor : newlyBornActors) 
		allActors.push_back(newActor);
	newlyBornActors.clear();

	//Update all the actors
	Actor * previous = NULL;
	for (auto it = allActors.begin(); it != allActors.end(); ) {
		previous = *it;
		if ((*it)->Update()) {
			//Unregister physics actors
			if (dynamic_cast<PhysicsActor*>(*it) != NULL)
				physics->UnregisterPhysicsActor((PhysicsActor*)*it);
			//Add it to the recently died list
			recentlyDied.push_back(*it);
			//erase from list
			it = allActors.erase(it);
		}
		else
			it++;
	}

	//Delete anything that previously died
	for (auto dead : previouslyDied)
		delete dead;
	previouslyDied.clear();
}

ActorPlayer * ActorSystem::Player() {
	return player;
}

//Get the AIDS
ActorAids * ActorSystem::Aids() {
	return aids;
}

//Do AOE damage
//to actors
//also flings actors
void ActorSystem::DoAOEDamage(vec3 at, float radius, float damage, PhysicsActor * damager) {
	for (auto actor : allActors) {
		PhysicsActor * damagableActor = dynamic_cast<PhysicsActor*>(actor);
		if (damagableActor == NULL)
			continue;
		//Now check distance
		float dist = glm::distance(at,damagableActor->GetPosition());
		if (dist > radius)
			continue;
		//Scale damage
		float actorDamage = ((1.0f - dist/radius)*.5f+.5f)*damage;
		damagableActor->Damage(damager,actorDamage);
		//And fling the actor some amount
		float fling = damage*.05f;
		//Check if the actor is at the epicenter
		vec3 damageDirection;
		if (damagableActor->GetPosition() == at)
			damageDirection = vec3(0,0,1);
		else
			damageDirection = glm::normalize(damagableActor->GetPosition() - at);
		damagableActor->velocity += damageDirection*fling;
	}
}

//Draw all the actors
void ActorSystem::Draw(ShaderGroup * shaders) {

	// Setup the mesh shader
	MaterialProgram * modelShader = (MaterialProgram *) shaders->GetShader("model");
	modelShader->UseProgram();

	for (unsigned int i = 0; i < allActors.size(); i++) 
		allActors[i]->Draw(modelShader);

	//Reset texture unit 0 to the active texture unit
	glActiveTexture(GL_TEXTURE0);

	//setup the effect shader
	GLEffectProgram * effectShader = (GLEffectProgram*) shaders->GetShader("effects");
	effectShader->UseProgram();

	for (unsigned int i = 0; i < allActors.size(); i++) 
		allActors[i]->Draw(effectShader);
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