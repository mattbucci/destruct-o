
#pragma once

#include "stdafx.h"
#include "Utilities.h"

class GLParticleProgram;

//Renders a max of PARTICLE_RENDER_SWEEP
//particles per opengl draw call
//note: no matter what particles in separate systems
//are automatically separate render sweeps
#define PARTICLE_RENDER_SWEEP 2000

class Particle;

class ParticleRenderer {
	GLuint vertexArray;
	GLuint stripedBuffer;

	PACK(
	struct particleChunk {
		Utilities::PODVec4 vertex;
		Utilities::PODVec2 textureCoordinate;
		Utilities::PODVec4 color;
		unsigned char vertNumber;
	});

	particleChunk vertices[PARTICLE_RENDER_SWEEP*6];

	//Render one sweep of particles
	void renderSweep(GLParticleProgram * shader, int particleCount);
public:
	ParticleRenderer();
	~ParticleRenderer();

	//Render all the given particles
	//particles is an array of size "size" of particle *
	void Render(GLParticleProgram * shader, Particle ** particles, unsigned int size);
}; 