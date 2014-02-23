#pragma once

class GLParticleProgram;
class ParticleRenderer;
class Particle;

#include "ContiguousList.h"
#include "ParticleData.h"

class ParticleSystem {
	friend class Particle;

	//The data describing this particle system
	ParticleData particleSystemDescription;

	//State
	double nextParticle;
	double creationTime;
	double deathAt;

	//The list of particles
	ContiguousList<Particle> particleList;
public:
	ParticleSystem(ParticleData particleSystemDescription,double now, double lifetime);
	~ParticleSystem();

	//Particle system position
	//particles spawn relative to this (but do not move relative to it)
	vec3 Position;

	//Update the particle emitter spawning paritcles
	//returns true if this particle system should be cleaned up
	bool UpdateEmitter(double now, double delta);
	//Draw particles for this particle system
	void Draw(ParticleRenderer * renderer, GLParticleProgram * shader);
};
 