#include "stdafx.h"
#include "ParticleRenderer.h"
#include "GLParticleProgram.h"
#include "Particle.h"

//Build the arrays to create the vertex group
ParticleRenderer::ParticleRenderer() {
	//Generate the opengl buffers representing this vertex group
	//will also need a buffer for textures in the future
	glGenBuffers(1,&vertexBuffer);
	glGenBuffers(1,&textureBuffer);
	glGenBuffers(1,&colorBuffer);
	glGenVertexArrays(1,&vertexArray);

	//Buffer junk data (doesn't matter if data is uninitialized)
	//This allows the allocation to happen once
	glBindBuffer ( GL_ARRAY_BUFFER, vertexBuffer );
	glBufferData ( GL_ARRAY_BUFFER, PARTICLE_RENDER_SWEEP*sizeof(vec4), vertices, GL_DYNAMIC_DRAW );
	glBindBuffer ( GL_ARRAY_BUFFER, textureBuffer ); 
	glBufferData ( GL_ARRAY_BUFFER, PARTICLE_RENDER_SWEEP*sizeof(vec2), textureCoordinates, GL_DYNAMIC_DRAW );
	glBindBuffer ( GL_ARRAY_BUFFER, colorBuffer ); 
	glBufferData ( GL_ARRAY_BUFFER, PARTICLE_RENDER_SWEEP*sizeof(vec4), colors, GL_DYNAMIC_DRAW );
}

ParticleRenderer::~ParticleRenderer() {
	glDeleteBuffers(1,&vertexBuffer);
	glDeleteBuffers(1,&textureBuffer);
	glDeleteBuffers(1,&colorBuffer);
	glDeleteVertexArrays(1,&vertexArray);
}

void ParticleRenderer::renderSweep(GLParticleProgram * shader, int particleCount) {
	//There are four vertices for every particle right now
	particleCount *= 6;

	//Rebind the array to bring them into the current context
	glBindVertexArray ( vertexArray );

	 //positions
	glBindBuffer ( GL_ARRAY_BUFFER, vertexBuffer );
	glBufferSubData ( GL_ARRAY_BUFFER, 0, particleCount*sizeof(vec4), vertices );
	glEnableVertexAttribArray ( shader->AttributeVertex() );
	glVertexAttribPointer ( shader->AttributeVertex(), 4, GL_FLOAT, GL_FALSE, 0, 0 );


	// texture coordinates
	glBindBuffer ( GL_ARRAY_BUFFER, textureBuffer ); 
	glBufferSubData ( GL_ARRAY_BUFFER, 0, particleCount*sizeof(vec2), textureCoordinates );
	glEnableVertexAttribArray ( shader->AttributeTexture() );
	glVertexAttribPointer ( shader->AttributeTexture(), 2, GL_FLOAT, GL_FALSE, 0, 0 );

	//colors
	glBindBuffer ( GL_ARRAY_BUFFER, colorBuffer ); 
	glBufferSubData ( GL_ARRAY_BUFFER, 0, particleCount*sizeof(vec4), colors );
	glEnableVertexAttribArray ( shader->AttributeColor() );
	glVertexAttribPointer ( shader->AttributeColor(), 4, GL_FLOAT, GL_FALSE, 0, 0 );

	glDrawArrays( GL_TRIANGLES, 0, particleCount );
}
	

void ParticleRenderer::Render(GLParticleProgram * shader, Particle ** particles, unsigned int size) {
	_ASSERTE(vertexBuffer >= 0);
	_ASSERTE(vertexArray >= 0);
	_ASSERTE(textureBuffer >= 0);

	//If there are no vertices, abort
	if (size == 0)
		return;

	//Take the data for each particle and generate the vertices represented by that data
	int curParticles = 0;
	for (int i = 0; i < size; i++) {
		//If you can't fit any more particles in the current sweep
		//execute it now
		if (curParticles >= PARTICLE_RENDER_SWEEP) {
			renderSweep(shader,curParticles);
			curParticles = 0;
		}
		//Generate the vertex data for the given particle
		Particle * p = particles[i];
		int offset = i*6;
		float halfScale = p->Scale/2.0;
		//Generate position data
		vertices[offset+0] = vec4(p->Position*vec3(1,1,1),halfScale);
		vertices[offset+1] = vec4(p->Position*vec3(1,-1,1),halfScale);
		vertices[offset+2] = vec4(p->Position*vec3(-1,1,1),halfScale);
		vertices[offset+3] = vec4(p->Position*vec3(-1,-1,1),halfScale);
		vertices[offset+4] = vertices[offset+1];
		vertices[offset+5] = vertices[offset+2];

		//Now figure out texture coordinate
		
		//textureCoordinates[offset+0] = 
		curParticles++;
	}
	//Do one last render sweep to cleanup anything that hasn't been drawn yet
	renderSweep(shader,curParticles);
}
