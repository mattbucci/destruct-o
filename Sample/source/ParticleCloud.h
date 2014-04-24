
#pragma once


#include "GL3DProgram.h"
#include "ContiguousList.h"
#include "ParticleRenderer.h"
#include "GameEventSubscriber.h"

class Particle;
class ParticleSystem;
class ParticleData;
class ShaderGroup;

class ActorSystem;
class PhysicsSystem;


class ParticleCloud : GameEventSubscriber {
	//All the particle systems in the game
	ContiguousList<ParticleSystem*> particles;

	//Loaded particle data
	map<string,ParticleData> loadedParticleData;

	//Retrieve a copy of a particle system
	//private because you're supposed to use subscriptions!
	ParticleData & GetParticleData(string systemFileName);

	//This is the renderer for particles
	//similar to the voxel system, it will probably be replaced
	//with an instanced version for non-android/up-to-date platforms
	ParticleRenderer renderer;
public:
	//Register particle events
	ParticleCloud(ActorSystem * actors, PhysicsSystem * physics);
	//Cleanup all systems
	~ParticleCloud();


	//Load particle systems
	//call once per build
	void LoadParticles();

	//Updates the contents of the particle cloud automatically
	void UpdateCloud();

	//Build a particle system with the given particle data
	//to last the specified amount of time
	ParticleSystem * BuildParticleSystem(const ParticleData & particleType, float lifeTime);

	//Destroy all particle systems that currently exist
	void Clear();

	//Draw the particle cloud using the special particle shader
	void Draw(ShaderGroup * shaders);

	//Destroy all the particles owned by a particular system
	//void CleanupSystem(ParticleSystem * system);

};
 