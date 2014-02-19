#include "stdafx.h"
#include "ParticleCloud.h"
#include "ParticleSystem.h"
#include "ShaderGroup.h"
#include "GLParticleProgram.h"
#include "GL3DProgram.h"

//Updates the contents of the particle cloud automatically
void ParticleCloud::UpdateCloud(double time, double delta) {

	//First update each particle
	for (auto it = particles.begin(); it != particles.end(); ) {
		ParticleSystem * current = *it;
		
		if (!current->UpdateEmitter(time,delta)) {
			//Cleanup that particle system
			delete current;
			//Remove it from the list
			it = particles.erase(it);
		}
		else
			//Move to the next particle
			it++;
	}
}
/*
void ParticleCloud::CleanupSystem(ParticleSystem * system) {
	if (particles.size() <= 0)
		//Nothing to update, go no further
		return;


	//Cleanup each particle
	Particle ** first = &particles[0];
	for (unsigned int i = 0; i < particles.size(); i++) {
		Particle * current = *(first+i);
		if ((current != NULL) && (current->owner == system)) {
			//Cleanup that particle
			delete current;
			//Mark the current entry as unused
			*(first+i) = NULL;
		}
	}
}
*/

//Draw the particle cloud using a special shader in the future
void ParticleCloud::Draw(ShaderGroup * shaders) {
	GL3DProgram * shader3d = (GL3DProgram *)shaders->GetShader("3d");
	GLParticleProgram * shaderParticles = (GLParticleProgram*)shaders->GetShader("particles");
	//Switch to the particle shader
	shaderParticles->UseProgram();
	//Apply camera data from the 3d shader
	shaderParticles->ApplyCamera(shader3d->Camera);

	//Draw each particle system
	for (auto system : particles)
		system->Draw();
}
