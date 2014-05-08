#include "stdafx.h"
#include "ParticleRenderer.h"
#include "GLParticleProgram.h"
#include "Particle.h"
#include "ParticleData.h"

//Build the arrays to create the vertex group
ParticleRenderer::ParticleRenderer() {
	//Generate the opengl buffers representing this vertex group
	//will also need a buffer for textures in the future
	glGenBuffers(1,&stripedBuffer);
    glGenBuffers(1,&indexBuffer);
	glGenVertexArrays(1,&vertexArray);

	//Buffer junk data (doesn't matter if data is uninitialized)
	//This allows the allocation to happen once
	glBindBuffer ( GL_ARRAY_BUFFER, stripedBuffer );
	glBufferData ( GL_ARRAY_BUFFER, PARTICLE_RENDER_SWEEP*4*sizeof(particleChunk), vertices, GL_DYNAMIC_DRAW );
    
    //Buffer junk data (doesn't matter if data is uninitialized)
	//This allows the allocation to happen once
	glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, indexBuffer );
	glBufferData ( GL_ELEMENT_ARRAY_BUFFER, PARTICLE_RENDER_SWEEP*6*sizeof(GLushort), indices, GL_DYNAMIC_DRAW );
}

ParticleRenderer::~ParticleRenderer() {

	glDeleteBuffers(1,&stripedBuffer);
	glDeleteVertexArrays(1,&vertexArray);
}

void ParticleRenderer::renderSweep(GLParticleProgram * shader, int particleCount) {
	// Rebind the array to bring them into the current context
	glBindVertexArray ( vertexArray );

	 //striped buffer
	glBindBuffer ( GL_ARRAY_BUFFER, stripedBuffer );
	glBufferSubData ( GL_ARRAY_BUFFER, 0, particleCount*sizeof(particleChunk)*4, vertices );

    // indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, particleCount*sizeof(GLushort)*6, indices);
    
	// texture coordinates
	glEnableVertexAttribArray ( shader->AttributeVertex() );
	glVertexAttribPointer ( shader->AttributeVertex(), 4, GL_FLOAT, GL_FALSE, sizeof(particleChunk), (void*)offsetof(particleChunk,vertex) );
    
	glEnableVertexAttribArray ( shader->AttributeTexture() );
	glVertexAttribPointer ( shader->AttributeTexture(), 2, GL_FLOAT, GL_FALSE, sizeof(particleChunk), (void*)offsetof(particleChunk,textureCoordinate) );
    
	glEnableVertexAttribArray ( shader->AttributeColor() );
	glVertexAttribPointer ( shader->AttributeColor(), 4, GL_FLOAT, GL_FALSE, sizeof(particleChunk), (void*)offsetof(particleChunk,color) );
    
	glEnableVertexAttribArray ( shader->AttributeVertexNumber() );
	glVertexAttribPointer ( shader->AttributeVertexNumber(), 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(particleChunk), (void*)offsetof(particleChunk,vertNumber) );

    // Draw the elements
	glDrawElements(GL_TRIANGLES, particleCount * 6, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);
}

void ParticleRenderer::Render(GLParticleProgram * shader, Particle * particles, unsigned int size) {

	//If there are no vertices, abort
	if (size == 0)
		return;

	//Take the data for each particle and generate the vertices represented by that data
	int curParticles = 0;
	for (unsigned int i = 0; i < size; i++) {
		//If you can't fit any more particles in the current sweep
		//execute it now
		if (curParticles >= PARTICLE_RENDER_SWEEP) {
			renderSweep(shader,curParticles);
			curParticles = 0;
		}
        
		//Generate the vertex data for the given particle
		Particle & p = particles[i];
		int offset = curParticles*4;
        int ioffset = curParticles*6;
		float halfScale = p.Scale/2.0f;
        
		//Generate position data
		vertices[offset+0].vertex = toPOD(vec4(p.Position,halfScale));
		vertices[offset+1].vertex = toPOD(vec4(p.Position,halfScale));
		vertices[offset+2].vertex = toPOD(vec4(p.Position,halfScale));
		vertices[offset+3].vertex = toPOD(vec4(p.Position,halfScale));


		//Now figure out texture coordinate
		vec2 texSize = vec2(1.0f/p.SystemData->Columns,1.0f/p.SystemData->Rows);
		vec2 texCorner = texSize * vec2(p.Frame % (int)p.SystemData->Columns, p.Frame / (int)p.SystemData->Columns);
		vertices[offset+0].textureCoordinate = toPOD(texCorner + texSize*vec2(0,0));
		vertices[offset+1].textureCoordinate = toPOD(texCorner + texSize*vec2(1,0));
		vertices[offset+2].textureCoordinate = toPOD(texCorner + texSize*vec2(0,1));
		vertices[offset+3].textureCoordinate = toPOD(texCorner + texSize*vec2(1,1));

		//finally color
		vertices[offset+0].color = vertices[offset+1].color = vertices[offset+2].color = vertices[offset+3].color = toPOD(p.Color);

		//vertex numbers
		vertices[offset+0].vertNumber = 0;
		vertices[offset+1].vertNumber = 1;
		vertices[offset+2].vertNumber = 2;
		vertices[offset+3].vertNumber = 3;
        
        // Generate the indices
        indices[ioffset+0] = offset + 0;
        indices[ioffset+1] = offset + 1;
        indices[ioffset+2] = offset + 2;
        indices[ioffset+3] = offset + 1;
        indices[ioffset+4] = offset + 2;
        indices[ioffset+5] = offset + 3;
		
		//Now increment which particle you're on
		curParticles++;
	}
	//Do one last render sweep to cleanup anything that hasn't been drawn yet
	renderSweep(shader,curParticles);
}
