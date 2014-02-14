
#pragma once


#include "GL3DProgram.h"


class Particle;
class ParticleSystem;

class ParticleCloud {
	//All the particles in the cloud
	//Null values automatically float to the "bottom" particles.length
	//and should be replaced with actual values
	//Resorted every frame (sloow)
	vector<Particle*> particles;
public:

	//Updates the contents of the particle cloud automatically
	void UpdateCloud(double time, double delta, vec3 cameraPosition);

	//Add a particle to the cloud
	void AddParticle(Particle * particle);

	//Draw the particle cloud using a special shader in the future
	void Draw(GL3DProgram * shaders);

	//Destroy all the particles owned by a particular system
	void CleanupSystem(ParticleSystem * system);
};
