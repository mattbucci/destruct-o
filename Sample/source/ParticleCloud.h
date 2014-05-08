
#pragma once


#include "GL3DProgram.h"
#include "ContiguousList.h"
#include "ParticleRenderer.h"
#include "GameEventSubscriber.h"
#include "Autocacher.h"
#include "ParticleData.h"
#include "TimeStepTracker.h"

class Particle;
class ParticleSystem;
class ShaderGroup;

class ActorSystem;
class PhysicsSystem;


class ParticleCloud : GameEventSubscriber, public Autocacher<ParticleData> {
	//All the particle systems in the game
	ContiguousList<ParticleSystem*> particles;

	//Loaded particle data
	map<string,ParticleData> loadedParticleData;
	
	//This is the renderer for particles
	//similar to the voxel system, it will probably be replaced
	//with an instanced version for non-android/up-to-date platforms
	ParticleRenderer renderer;

	//Track the update,draw times of particles
	TimeStepTracker updateTracker;
	TimeStepTracker drawTracker;

	//Erase some particle systems randomly based off of the given recommendation
	void enactRecommendation(float recommendation, bool updateRecommendation);
protected:
	//Cache particle data
	ParticleData cacheItem(string path) override;

public:
	//Register particle events
	ParticleCloud(ActorSystem * actors, PhysicsSystem * physics);
	//Cleanup all systems
	~ParticleCloud();

	//Updates the contents of the particle cloud automatically
	void UpdateCloud();

	//Build a particle system with the given particle data
	//to last the specified amount of time
	void BuildParticleSystem(const ParticleData & particleType, vec3 pos, float lifeTime);

	//Destroy all particle systems that currently exist
	void Clear();

	//Draw the particle cloud using the special particle shader
	void Draw(ShaderGroup * shaders);

	//Destroy all the particles owned by a particular system
	//void CleanupSystem(ParticleSystem * system);

};
 