#include "stdafx.h"
#include "ParticleSystem.h"
#include "Particle.h"
#include "BaseFrame.h"
#include "Frames.h"
#include "ParticleData.h"

#include "GLTexture.h"
#include "TextureCache.h"
#include "GLParticleProgram.h"
#include "VoxEngine.h"

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
	particleList.clear();
}

//Update the particle emitter spawning particles
bool ParticleSystem::UpdateEmitter() {
	//Adjust the time so it's relative to you
	double now = Game()->Now();
	double systemTime = now-creationTime;
	float systemLifeFactor = (float)(systemTime/(deathAt-creationTime));
	if (deathAt < 0)
		systemLifeFactor = 1;
	
	//A negative death time indicates infinite life
	if ((deathAt < 0) || (deathAt > now)) {
		if (nextParticle == 0)
			nextParticle = systemTime;

		//Generate as many new particles as are necessary to catch up
		float generationRate = particleSystemDescription.GenerationRate.ValueAtSequence(systemLifeFactor);
		//Scale generation rate with particle system options
		//generation rate goes from 25% to 100%
		generationRate *= (VoxEngine::GlobalSavedData.GameOptions.ParticleQuality*.75f+.25f);
		//Check if it's time to spawn a new particle
		double perParticle = 1.0f/generationRate;
		while (nextParticle < systemTime) {
			nextParticle += perParticle;
			particleList.push_back(Particle(now,systemLifeFactor,this,&particleSystemDescription));
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
		Particle & p = *it;
		if (p.Update(now)) {
			//The particle requested death, cleanup
			it = particleList.erase(it);
		}
		else
			it++;
	}

	//Keep on living
	return false;
}

//Handle particle system material and draw style now
void ParticleSystem::Draw(ParticleRenderer * renderer, GLParticleProgram * shader, vec3 cameraZAxis) {
	//Enforce the selected material
	switch(particleSystemDescription.MaterialStyle) {
	case ParticleData::NONE:
		glDepthMask(GL_TRUE);
		glDisable (GL_BLEND);
		break;
	case ParticleData::ADDITIVE:
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		break;
	case ParticleData::BLEND:
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	case ParticleData::SCREEN:
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
		break;
	}
	//Sort if necessary
	if (particleSystemDescription.MaterialStyle == ParticleData::BLEND) {
		particleList.sort([cameraZAxis](const Particle & a, const Particle & b) {
			return glm::dot(a.Position, cameraZAxis) < glm::dot(b.Position, cameraZAxis);
		});
	}
	
	//Enable the texture used for this particle system
	GLTexture * texture = CurrentSystem->Textures.GetTexture<GLTexture>(particleSystemDescription.MaterialTexture);
	texture->Bind();
	//Call the renderer to render all the particles
	renderer->Render(shader,&particleList[0],particleList.size());
}

size_t ParticleSystem::ParticleCount() const
{
	return particleList.size();
}