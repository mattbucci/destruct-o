#include "stdafx.h"
#include "ParticleCloud.h"
#include "Particle.h"

		//All the particles in the cloud
		//Null values automatically float to the "bottom" particles.length
		//and should be replaced with actual values
		//Resorted every frame (sloow)
		//vector<Particle*> particles;

//Updates the contents of the particle cloud automatically
void ParticleCloud::UpdateCloud(double time, double delta, vec3 cameraPosition) {

	if (particles.size() <= 0)
		//Nothing to update, go no further
		return;

	int activeParticles = 0;
	//First update each particle
	Particle ** first = &particles[0];
	for (int i = 0; i < particles.size(); i++) {
		Particle * current = *(first+i);
		//Done
		if (current == NULL)
			continue;

		if (!current->Update(time,delta)) {
			//Cleanup that particle
			delete current;
			//Mark the current entry as unused
			*(first+i) = NULL;
		}
		else
			//Mark the current particle as active
			activeParticles++;
	}

	//second sort the particles so that the closer particles are closer here
	sort(particles.begin(),particles.end(),
		[cameraPosition](Particle* a, Particle* b) {
			if ((a == NULL) && (b == NULL))
				//If they're both null then they're equal
				return false;
			else if ((a == NULL) || (b == NULL))
				//If the first one is null then it's "greater" than the second one
				return (b == NULL);
			else {
				//Otherwise sort by distance from the camera
				vec3 aDist = cameraPosition - a->position;
				vec3 bDist = cameraPosition - a->position;
				return (glm::dot(aDist,aDist) > glm::dot(bDist,bDist));
			}
		});
	//Decrease the size of the particle array when possible
	if (activeParticles*2.0 < (double)particles.size()) {
		particles.resize((int)(activeParticles*1.25));
	}
}

void ParticleCloud::CleanupSystem(ParticleSystem * system) {
	if (particles.size() <= 0)
		//Nothing to update, go no further
		return;


	//Cleanup each particle
	Particle ** first = &particles[0];
	for (int i = 0; i < particles.size(); i++) {
		Particle * current = *(first+i);
		if ((current != NULL) && (current->owner == system)) {
			//Cleanup that particle
			delete current;
			//Mark the current entry as unused
			*(first+i) = NULL;
		}
	}
}

//Add a particle to the cloud
void ParticleCloud::AddParticle(Particle * particle) {
	if (particles.size() > 0) {
		//Search for the first empty slot starting from the back, sorting will pick you up and move you to the front
		//however you won't update during the first frame
		Particle ** first = &particles[0];
		for (int i = particles.size()-1; i >= 0; i--) {
			Particle * current = *(first+i);
			if (current == NULL) {
				*(first+i) = particle;
				return;
			}
		}
	}

	//No room in array, push back a new particle and an empty slot to reduce iterations when adding
	//many particles
	particles.push_back(particle);
	particles.push_back(NULL);
}

//Draw the particle cloud using a special shader in the future
void ParticleCloud::Draw(GL3DProgram * shaders) {

	if (particles.size() <= 0)
		//Nothing to update, go no further
		return;

	mat4 viewMatrix;
	shaders->Camera.CopyMatricies(&viewMatrix,NULL);

	Particle ** first = &particles[0];
	for (int i = 0; i < particles.size(); i++) {
		Particle * current = *(first+i);
		//Done
		if (current == NULL)
			break;
		//else draw the current particle
		current->Draw(viewMatrix,shaders);
	}
}
