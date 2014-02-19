#include "stdafx.h"
#include "ParticleSystem.h"
#include "Particle.h"
#include "BaseFrame.h"
#include "Frames.h"
#include "ParticleData.h"

ParticleSystem::ParticleSystem(ParticleData * particleSystemDescription,double now) {
	this->particleSystemDescription = particleSystemDescription;
	this->creationTime = now;
	nextParticle = 0;
}
ParticleSystem::~ParticleSystem() {
}

//Update the particle emitter spawning particles
bool ParticleSystem::UpdateEmitter(double now, double delta) {
	double gameTime = now;
	BaseFrame * game = (BaseFrame*)CurrentSystem;
	//Adjust the time so it's relative to you
	now -= creationTime;

	if (nextParticle == 0)
		nextParticle = now;

	//Generate as many new particles as are necessary to catch up
	double perParticle = 1.0f/particleSystemDescription->GenerationRate.ValueAtSequence((float)now);
	while (nextParticle < now) {
		nextParticle += perParticle;
		Particle * p = new Particle(gameTime,(float)now,this,particleSystemDescription);
		game->Particles.AddParticle(p);
	}

}
//Particles are drawn as a cloud after the models are drawn 
//and so this call doesn't do anything for particles
void ParticleSystem::Draw(GL3DProgram * shaders) {

}