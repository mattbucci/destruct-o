#include "stdafx.h"
#include "GLDynamicVertexGroup.h"

//Build the arrays to create the vertex group
GLDynamicVertexGroup::GLDynamicVertexGroup() {
	vertices = NULL;
	vertexNormals = NULL;
	sizeChanged = true;

	//Generate the opengl buffers representing this vertex group
	//will also need a buffer for textures in the future
	glGenBuffers(1,&vertexBuffer);
	glGenBuffers(1,&normalBuffer);
	glGenBuffers(1,&textureBuffer);
	glGenVertexArrays(1,&vertexArray);
}

void GLDynamicVertexGroup::UpdateGroup(GLenum gltype, int vertexCount) {
	//Use C allocaiton so I can use realloc()
	if (vertices == NULL) {
		vertices = (vec3*)malloc(sizeof(vec3)*vertexCount);
		vertexNormals = (vec3*)malloc(sizeof(vec3)*vertexCount);
		textureCoords = (vec2*)malloc(sizeof(vec2)*vertexCount);
		memset(vertexNormals,0,sizeof(vertexNormals));
	}
	else {
		vertices = (vec3*)realloc(vertices, sizeof(vec3)*vertexCount);
		vertexNormals = (vec3*)realloc(vertexNormals, sizeof(vec3)*vertexCount);
		textureCoords = (vec2*)realloc(textureCoords,sizeof(vec2)*vertexCount);
	}

	this->gltype = gltype;
	this->vertexCount = vertexCount;
	sizeChanged = true;
}

GLDynamicVertexGroup::~GLDynamicVertexGroup() {
	glDeleteBuffers(1,&vertexBuffer);
	glDeleteBuffers(1,&normalBuffer);
	glDeleteBuffers(1,&textureBuffer);
	glDeleteVertexArrays(1,&vertexArray);

	free(vertices);
	free(vertexNormals);
	free(textureCoords);
}

	
//For modifying vertices
vec3 & GLDynamicVertexGroup::vat(const int index) {
	_ASSERTE((index >= 0) && (index < vertexCount));
	return vertices[index];
}

vec3 & GLDynamicVertexGroup::nat(const int index) {
	_ASSERTE((index >= 0) && (index < vertexCount));
	return vertexNormals[index];
}

vec2 & GLDynamicVertexGroup::xat(const int index) {
	_ASSERTE((index >= 0) && (index < vertexCount));
	return textureCoords[index];
}



void GLDynamicVertexGroup::Draw() {
	_ASSERTE(vertexBuffer >= 0);
	_ASSERTE(vertexArray >= 0);
	_ASSERTE(normalBuffer >= 0);

	//Rebind the array to bring them into the current context
	glBindVertexArray ( vertexArray );

	if (sizeChanged) {
		//positions
		glBindBuffer ( GL_ARRAY_BUFFER, vertexBuffer );
		glBufferData ( GL_ARRAY_BUFFER, vertexCount*sizeof(vec3), vertices, GL_DYNAMIC_READ );
		glEnableVertexAttribArray ( 0 );
		glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );

		//Normals
		glBindBuffer ( GL_ARRAY_BUFFER, normalBuffer ); 
		glBufferData ( GL_ARRAY_BUFFER, vertexCount*sizeof(vec3), vertexNormals, GL_DYNAMIC_READ );
		glEnableVertexAttribArray ( 1 );
		glVertexAttribPointer ( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );

		//texture coordinates
		glBindBuffer ( GL_ARRAY_BUFFER, textureBuffer ); 
		glBufferData ( GL_ARRAY_BUFFER, vertexCount*sizeof(vec2), textureCoords, GL_DYNAMIC_READ );
		glEnableVertexAttribArray ( 2 );
		glVertexAttribPointer ( 2, 2, GL_FLOAT, GL_FALSE, 0, 0 ); 

		sizeChanged = false;
	}
	else {
		 //positions
		glBindBuffer ( GL_ARRAY_BUFFER, vertexBuffer );
		glBufferSubData ( GL_ARRAY_BUFFER, 0, vertexCount*sizeof(vec3), vertices);
		glEnableVertexAttribArray ( 0 );
		glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );

		//Normals
		glBindBuffer ( GL_ARRAY_BUFFER, normalBuffer ); 
		glBufferSubData ( GL_ARRAY_BUFFER, 0, vertexCount*sizeof(vec3), vertexNormals);
		glEnableVertexAttribArray ( 1 );
		glVertexAttribPointer ( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );

		//texture coordinates
		glBindBuffer ( GL_ARRAY_BUFFER, textureBuffer ); 
		glBufferSubData ( GL_ARRAY_BUFFER, 0, vertexCount*sizeof(vec2), textureCoords);
		glEnableVertexAttribArray ( 2 );
		glVertexAttribPointer ( 2, 2, GL_FLOAT, GL_FALSE, 0, 0 ); 
	}



	glDrawArrays( gltype, 0, vertexCount );
}
