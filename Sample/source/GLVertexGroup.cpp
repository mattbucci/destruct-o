#include "stdafx.h"
#include "GLVertexGroup.h"
#include "GLModel.h"


//Build the arrays to create the vertex group
GLVertexGroup::GLVertexGroup(GLenum gltype, int vertexCount) {
	vertices = new vec3[vertexCount];
	vertexNormals = new vec3[vertexCount];

	memset(vertexNormals,0,sizeof(vertexNormals));

	this->gltype = gltype;
	this->vertexCount = vertexCount;

	//Generate the opengl buffers representing this vertex group
	//will also need a buffer for textures in the future
	glGenBuffers(1,&vertexBuffer);
	glGenBuffers(1,&normalBuffer);
	glGenVertexArrays(1,&vertexArray);
}

GLVertexGroup::~GLVertexGroup() {
	glDeleteBuffers(1,&vertexBuffer);
	glDeleteBuffers(1,&normalBuffer);
	glDeleteVertexArrays(1,&vertexArray);

	delete [] vertices;
	delete [] vertexNormals;
}
	
//For modifying vertices
vec3 & GLVertexGroup::vat(const int index) {
	return vertices[index];
}

vec3 & GLVertexGroup::nat(const int index) {
	return vertexNormals[index];
}


void GLVertexGroup::Draw() {
	_ASSERTE(vertexBuffer >= 0);
	_ASSERTE(vertexArray >= 0);
	_ASSERTE(normalBuffer >= 0);

	//Rebind the array to bring them into the current context
	glBindVertexArray ( vertexArray );

	 //positions
	glBindBuffer ( GL_ARRAY_BUFFER, vertexBuffer );
	glBufferData ( GL_ARRAY_BUFFER, vertexCount*sizeof(vec3), vertices, GL_DYNAMIC_READ );
	glEnableVertexAttribArray ( 0 );
	glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );

	// colors - soon to be replaced by solid colors only using the shader
	glBindBuffer ( GL_ARRAY_BUFFER, normalBuffer ); 
	glBufferData ( GL_ARRAY_BUFFER, vertexCount*sizeof(vec3), vertexNormals, GL_DYNAMIC_READ );
	glEnableVertexAttribArray ( 1 );
	glVertexAttribPointer ( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );
   
	glDrawArrays( gltype, 0, vertexCount );
}
