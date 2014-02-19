
#pragma once


#include "GL3DProgram.h"
#include "ContiguousList.h"


class Particle;
class ParticleSystem;
class ParticleData;
class ShaderGroup;

class ParticleCloud {
	//All the particle systems in the game
	ContiguousList<ParticleSystem> particles;
public:

	//Updates the contents of the particle cloud automatically
	void UpdateCloud(double time, double delta);

	//Build a particle system with the given particle data
	//to last the specified amount of time
	ParticleSystem * BuildParticleSystem(const ParticleData & particleType, float lifeTime);

	//Draw the particle cloud using the special particle shader
	void Draw(ShaderGroup * shaders);

	//Destroy all the particles owned by a particular system
	//void CleanupSystem(ParticleSystem * system);
};
