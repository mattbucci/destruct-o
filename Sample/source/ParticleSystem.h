#pragma once

class ParticleData;
class GL3DProgram;
class ModelData;

#include "ContiguousList.h"

class ParticleSystem {
	friend class Particle;

	//The data describing this particle system
	ParticleData * particleSystemDescription;
	ModelData * parent;

	//State
	double nextParticle;
	double creationTime;

	//The list of particles
	ContiguousList<Particle*> particleList;
public:
	ParticleSystem(ParticleData * particleSystemDescription,double now);
	~ParticleSystem();

	//Particle system position
	//particles spawn relative to this (but do not move relative to it)
	vec3 Position;

	//Update the particle emitter spawning paritcles
	void UpdateEmitter(double now);
	//Particles are drawn as a cloud after the models are drawn 
	//and so this call doesn't do anything for particles
	void Draw(GL3DProgram * shaders, double now);
};
