#include "stdafx.h"
#include "GL2DVertexGroup.h"
#include "GLModel.h"
#include "GL2DProgram.h"

//Build the arrays to create the vertex group
GL2DVertexGroup::GL2DVertexGroup(GLenum gltype, int vertexCount) {
	vertices = new vec2[vertexCount];
	vertexTextureCoords = new vec2[vertexCount];

	for (int i = 0; i < vertexCount; i++)
		vertexTextureCoords[i] = vec2(1.0f,1.0f);

	this->gltype = gltype;
	this->vertexCount = vertexCount;

	//If there are no vertices, abort
	if (vertexCount == 0)
		return;

	dirty = true;

	//Generate the opengl buffers representing this vertex group
	//will also need a buffer for textures in the future
	glGenBuffers(1,&vertexBuffer);
	glGenBuffers(1,&textureBuffer);
	glGenVertexArrays(1,&vertexArray);

	//Buffer junk data (doesn't matter if data is uninitialized)
	//This allows the allocation to happen once
	glBindBuffer ( GL_ARRAY_BUFFER, vertexBuffer );
	glBufferData ( GL_ARRAY_BUFFER, vertexCount*sizeof(vec2), vertices, GL_DYNAMIC_DRAW );
	glBindBuffer ( GL_ARRAY_BUFFER, textureBuffer ); 
	glBufferData ( GL_ARRAY_BUFFER, vertexCount*sizeof(vec2), vertexTextureCoords, GL_DYNAMIC_DRAW );
}

void GL2DVertexGroup::ResizeVertexGroup(int newSize) {
	delete[] vertices;
	delete[] vertexTextureCoords;

	this->vertexCount = newSize;

	vertices = new vec2[vertexCount];
	vertexTextureCoords = new vec2[vertexCount];
	//Texture coordinates must be valid or they could cause silent
	//errors in the shaders
	for (int i = 0; i < vertexCount; i++)
		vertexTextureCoords[i] = vec2(1.0f,1.0f);
	//Buffer junk data (doesn't matter if data is uninitialized)
	//This allows the allocation to happen once
	glBindBuffer ( GL_ARRAY_BUFFER, vertexBuffer );
	glBufferData ( GL_ARRAY_BUFFER, vertexCount*sizeof(vec2), vertices, GL_DYNAMIC_DRAW );
	glBindBuffer ( GL_ARRAY_BUFFER, textureBuffer ); 
	glBufferData ( GL_ARRAY_BUFFER, vertexCount*sizeof(vec2), vertexTextureCoords, GL_DYNAMIC_DRAW );
}

GL2DVertexGroup::~GL2DVertexGroup() {
	glDeleteBuffers(1,&vertexBuffer);
	glDeleteBuffers(1,&textureBuffer);
	glDeleteVertexArrays(1,&vertexArray);

	delete [] vertices;
	delete [] vertexTextureCoords;
}
	
//For modifying vertices
void GL2DVertexGroup::svat(const int index, vec2 vertex) {
	if (dirty || vertices[index] != vertex) {
		dirty = true;
		vertices[index] = vertex;
	}
}

void GL2DVertexGroup::sxat(const int index, vec2 textureCoordinate) {
	if (dirty || vertexTextureCoords[index] != textureCoordinate) {
		dirty = true;
		vertexTextureCoords[index] = textureCoordinate;
	}
}

vec2 GL2DVertexGroup::svat(const int index) {
	return vertices[index];
}
vec2 GL2DVertexGroup::sxat(const int index) {
	return vertexTextureCoords[index];
}

void GL2DVertexGroup::Draw(GL2DProgram * shader) {
	_ASSERTE(vertexBuffer >= 0);
	_ASSERTE(vertexArray >= 0);
	_ASSERTE(textureBuffer >= 0);

	//If there are no vertices, abort
	if (vertexCount == 0)
		return;

	//Rebind the array to bring them into the current context
	glBindVertexArray ( vertexArray );

	if (dirty) {
		dirty = false;
		 //positions
		glBindBuffer ( GL_ARRAY_BUFFER, vertexBuffer );
		glBufferSubData ( GL_ARRAY_BUFFER, 0, vertexCount*sizeof(vec2), vertices );
		glEnableVertexAttribArray ( shader->AttributePosition() );
		glVertexAttribPointer ( shader->AttributePosition(), 2, GL_FLOAT, GL_FALSE, 0, 0 );


		// texture coordinates
		glBindBuffer ( GL_ARRAY_BUFFER, textureBuffer ); 
		glBufferSubData ( GL_ARRAY_BUFFER, 0, vertexCount*sizeof(vec2), vertexTextureCoords );
		glEnableVertexAttribArray ( shader->AttributeTexture() );
		glVertexAttribPointer ( shader->AttributeTexture(), 2, GL_FLOAT, GL_FALSE, 0, 0 );
	}
	else {
		//Nothing necessary?
	}


	glDrawArrays( gltype, 0, vertexCount );
}
