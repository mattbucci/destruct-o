#include "stdafx.h"
#include "ParticleCloud.h"
#include "ParticleSystem.h"
#include "ShaderGroup.h"
#include "GLParticleProgram.h"
#include "GL3DProgram.h"
#include "VoxEngine.h"

//Updates the contents of the particle cloud automatically
void ParticleCloud::UpdateCloud(double time, double delta) {

	//First update each particle
	for (auto it = particles.begin(); it != particles.end(); ) {
		ParticleSystem * current = *it;
		
		if (current->UpdateEmitter(time,delta)) {
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

ParticleSystem * ParticleCloud::BuildParticleSystem(const ParticleData & particleType,float lifeTime) {
	ParticleSystem * ps = new ParticleSystem(particleType,VoxEngine::GetGameSimTime(),lifeTime);
	particles.push_back(ps);
	return ps;
}

void ParticleCloud::Clear() {
	for (auto particleSystem : particles)
		delete particleSystem;
	particles.clear();
}


//Draw the particle cloud using a special shader in the future
void ParticleCloud::Draw(ShaderGroup * shaders) {
	GL3DProgram * shader3d = (GL3DProgram *)shaders->GetShader("3d");
	GLParticleProgram * shaderParticles = (GLParticleProgram*)shaders->GetShader("particles");
	//Switch to the particle shader
	shaderParticles->UseProgram();
	//Apply camera data from the 3d shader
	shaderParticles->ApplyCamera(shader3d->Camera);

    vec3 cameraZ = shader3d->Camera.GetZAxis();

	//Draw each particle system
	for (auto system : particles)
        system->Draw(&renderer, shaderParticles, cameraZ);

	//Some particle systems disable depth
	//re-enable it before continuing
	glDepthMask(GL_TRUE);
}
