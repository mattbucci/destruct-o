
#pragma once


#include "GL3DProgram.h"
#include "ContiguousList.h"
#include "ParticleRenderer.h"


class Particle;
class ParticleSystem;
class ParticleData;
class ShaderGroup;

class ParticleCloud {
	//All the particle systems in the game
	ContiguousList<ParticleSystem*> particles;

	//Loaded particle data
	map<string,ParticleData> loadedParticleData;

	//This is the renderer for particles
	//similar to the voxel system, it will probably be replaced
	//with an instanced version for non-android/up-to-date platforms
	ParticleRenderer renderer;
public:
	//Load particle systems
	//call once per build
	void LoadParticles();

	//Updates the contents of the particle cloud automatically
	void UpdateCloud();

	//Build a particle system with the given particle data
	//to last the specified amount of time
	ParticleSystem * BuildParticleSystem(const ParticleData & particleType, float lifeTime);

	//Retrieve a copy of a particle system
	ParticleData GetParticleData(string systemFileName);

	//Destroy all particle systems that currently exist
	void Clear();

	//Draw the particle cloud using the special particle shader
	void Draw(ShaderGroup * shaders);

	//Destroy all the particles owned by a particular system
	//void CleanupSystem(ParticleSystem * system);

};
 