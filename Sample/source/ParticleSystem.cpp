#include "stdafx.h"
#include "ParticleSystem.h"
#include "Particle.h"
#include "BaseFrame.h"
#include "Frames.h"
#include "ParticleData.h"

#include "GLTexture.h"
#include "TextureCache.h"

ParticleSystem::ParticleSystem(ParticleData particleSystemDescription,double now, double lifetime) {
	this->particleSystemDescription = particleSystemDescription;
	this->creationTime = now;
	if (lifetime < 0)
		deathAt = -1;
	else
		deathAt = creationTime+lifetime;
	nextParticle = 0;
}
ParticleSystem::~ParticleSystem() {
}

//Update the particle emitter spawning particles
bool ParticleSystem::UpdateEmitter(double now, double delta) {
	//Adjust the time so it's relative to you
	double systemTime = now-creationTime;
	BaseFrame * game = (BaseFrame*)CurrentSystem;
	
	//A negative death time indicates infinite life
	if ((deathAt < 0) || (deathAt > now)) {
		if (nextParticle == 0)
			nextParticle = systemTime;

		//Generate as many new particles as are necessary to catch up
		double perParticle = 1.0f/particleSystemDescription.GenerationRate.ValueAtSequence((float)now);
		while (nextParticle < systemTime) {
			nextParticle += perParticle;
			Particle * p = new Particle(now,(float)systemTime,this,&particleSystemDescription);
			particleList.push_back(p);
		}
	}
	else {
		//Time for death, stop generating particles, when all the particles are gone
		//request your own destruction
		if (particleList.size() <= 0)
			return true;
	}

	//Update all the particles
	for (auto it = particleList.begin(); it != particleList.end();) {
		Particle * p = *it;
		if (p->Update(now,delta)) {
			//The particle requested death, cleanup
			delete p;
			it = particleList.erase(it);
		}
		else
			it++;
	}

	//Keep on living
	return false;
}

//Handle particle system material and draw style now
void ParticleSystem::Draw(ParticleRenderer * renderer, GLParticleProgram * shader) {
	//Enforce the selected material
	switch(particleSystemDescription.MaterialStyle) {
	case ParticleData::NONE:
		glDisable (GL_BLEND);
		break;
	case ParticleData::ADDITIVE:
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_COLOR, GL_ONE);
		break;
	case ParticleData::BLEND:
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		break;
	case ParticleData::SCREEN:
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
		break;
	}
    
// Was defined in every single case
#if !(defined __MOBILE__)
    // Not supported on mobile
    glDisable(GL_ALPHA_TEST);
#endif
    
	//Enable the texture used for this particle system
	GLTexture * texture = CurrentSystem->Textures.GetTexture(particleSystemDescription.MaterialTexture);
	texture->Bind();
	//Call the renderer to render all the particles
	renderer->Render(shader,&particleList[0],particleList.size());
} 