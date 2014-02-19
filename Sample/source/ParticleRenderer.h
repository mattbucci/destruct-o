
#pragma once

#include "stdafx.h"

class GLParticleProgram;

//Renders a max of PARTICLE_RENDER_SWEEP
//particles per opengl draw call
//note: no matter what particles in separate systems
//are automatically separate render sweeps
#define PARTICLE_RENDER_SWEEP 2000

class Particle;

class ParticleRenderer {
	GLuint vertexArray;
	GLuint vertexBuffer;
	GLuint textureBuffer;
	GLuint colorBuffer;

	vec3 vertices[PARTICLE_RENDER_SWEEP*4];
	vec2 textureCoordinates[PARTICLE_RENDER_SWEEP*4];
	vec4 colors[PARTICLE_RENDER_SWEEP*4];

	//Render one sweep of particles
	void renderSweep(GLParticleProgram * shader, int particleCount);
public:
	ParticleRenderer();
	~ParticleRenderer();

	//Render all the given particles
	//particles is an array of size "size" of particle *
	void Render(GLParticleProgram * shader, Particle ** particles, unsigned int size);
}; 