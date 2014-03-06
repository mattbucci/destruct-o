#include "stdafx.h"
#include "GLDynamicVertexGroup.h"
#include "GL3DProgram.h"

#ifdef __ANDROID__

//Build the arrays to create the vertex group
GLDynamicVertexGroup::GLDynamicVertexGroup() {
	vertices = NULL;
	vertexNormals = NULL;

	sizeChanged = true;
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



void GLDynamicVertexGroup::Draw(GL3DProgram * shader) {
	_ASSERTE(vertexBuffer >= 0);
	_ASSERTE(vertexArray >= 0);
	_ASSERTE(normalBuffer >= 0);
	_ASSERTE(positionBuffer >= 0);

	vector<vec4> positions(vertexCount);

	//vertices
	glEnableVertexAttribArray ( shader->AttributeVertex() );
	glVertexAttribPointer ( shader->AttributeVertex(), 3, GL_FLOAT, GL_FALSE, 0, vertices );

	//Normals
	glEnableVertexAttribArray ( shader->AttributeNormal() );
	glVertexAttribPointer ( shader->AttributeNormal(), 3, GL_FLOAT, GL_FALSE, 0, vertexNormals );

	//texture coordinates
	glEnableVertexAttribArray ( shader->AttributeTexture() );
	glVertexAttribPointer ( shader->AttributeTexture(), 2, GL_FLOAT, GL_FALSE, 0, textureCoords ); 


	//The GLDynamicVertexGroup does not use the position attribute
	//so instead it simply pushes 0 for all the positions after the size changes
	//the only consequence of this is that all texture coordinates must be from 0 to 4

	//vec4 automatically initializes to 0,0,0,0
		

	//position
	glBindBuffer ( GL_ARRAY_BUFFER, positionBuffer ); 
	glBufferData ( GL_ARRAY_BUFFER, vertexCount*sizeof(vec4), &positions[0], GL_DYNAMIC_DRAW );
	glEnableVertexAttribArray ( shader->AttributePosition() );
	glVertexAttribPointer ( shader->AttributePosition(), 4, GL_FLOAT, GL_FALSE, 0, 0 ); 


	glDrawArrays( gltype, 0, vertexCount );
}

#else

//Build the arrays to create the vertex group
GLDynamicVertexGroup::GLDynamicVertexGroup() {
	vertices = NULL;
	vertexNormals = NULL;

	//Generate the opengl buffers representing this vertex group
	//will also need a buffer for textures in the future
	glGenBuffers(1,&vertexBuffer);
	glGenBuffers(1,&normalBuffer);
	glGenBuffers(1,&textureBuffer);
	glGenBuffers(1,&positionBuffer);
	glGenVertexArrays(1,&vertexArray);

	sizeChanged = true;
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
	glDeleteBuffers(1,&positionBuffer);
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



void GLDynamicVertexGroup::Draw(GL3DProgram * shader) {
	_ASSERTE(vertexBuffer >= 0);
	_ASSERTE(vertexArray >= 0);
	_ASSERTE(normalBuffer >= 0);
	_ASSERTE(positionBuffer >= 0);

	//Rebind the array to bring them into the current context
	glBindVertexArray ( vertexArray );

	if (sizeChanged) {
		//vertices
		glBindBuffer ( GL_ARRAY_BUFFER, vertexBuffer );
		glBufferData ( GL_ARRAY_BUFFER, vertexCount*sizeof(vec3), vertices, GL_DYNAMIC_DRAW );
		glEnableVertexAttribArray ( shader->AttributeVertex() );
		glVertexAttribPointer ( shader->AttributeVertex(), 3, GL_FLOAT, GL_FALSE, 0, 0 );

		//Normals
		glBindBuffer ( GL_ARRAY_BUFFER, normalBuffer ); 
		glBufferData ( GL_ARRAY_BUFFER, vertexCount*sizeof(vec3), vertexNormals, GL_DYNAMIC_DRAW );
		glEnableVertexAttribArray ( shader->AttributeNormal() );
		glVertexAttribPointer ( shader->AttributeNormal(), 3, GL_FLOAT, GL_FALSE, 0, 0 );

		//texture coordinates
		glBindBuffer ( GL_ARRAY_BUFFER, textureBuffer ); 
		glBufferData ( GL_ARRAY_BUFFER, vertexCount*sizeof(vec2), textureCoords, GL_DYNAMIC_DRAW );
		glEnableVertexAttribArray ( shader->AttributeTexture() );
		glVertexAttribPointer ( shader->AttributeTexture(), 2, GL_FLOAT, GL_FALSE, 0, 0 ); 


		//The GLDynamicVertexGroup does not use the position attribute
		//so instead it simply pushes 0 for all the positions after the size changes
		//the only consequence of this is that all texture coordinates must be from 0 to 4

		//vec4 automatically initializes to 0,0,0,0
		vector<vec4> positions(vertexCount);

		//position
		glBindBuffer ( GL_ARRAY_BUFFER, positionBuffer ); 
		glBufferData ( GL_ARRAY_BUFFER, vertexCount*sizeof(vec4), &positions[0], GL_DYNAMIC_DRAW );
		glEnableVertexAttribArray ( shader->AttributePosition() );
		glVertexAttribPointer ( shader->AttributePosition(), 4, GL_FLOAT, GL_FALSE, 0, 0 ); 

		sizeChanged =false;
	}
	else {
		//vertices
		glBindBuffer ( GL_ARRAY_BUFFER, vertexBuffer );
		glBufferSubData ( GL_ARRAY_BUFFER, 0, vertexCount*sizeof(vec3), vertices);
		glEnableVertexAttribArray ( shader->AttributeVertex() );
		glVertexAttribPointer ( shader->AttributeVertex(), 3, GL_FLOAT, GL_FALSE, 0, 0 );

		//Normals
		glBindBuffer ( GL_ARRAY_BUFFER, normalBuffer ); 
		glBufferSubData ( GL_ARRAY_BUFFER, 0, vertexCount*sizeof(vec3), vertexNormals );
		glEnableVertexAttribArray ( shader->AttributeNormal() );
		glVertexAttribPointer ( shader->AttributeNormal(), 3, GL_FLOAT, GL_FALSE, 0, 0 );

		//texture coordinates
		glBindBuffer ( GL_ARRAY_BUFFER, textureBuffer ); 
		glBufferSubData ( GL_ARRAY_BUFFER, 0, vertexCount*sizeof(vec2), textureCoords);
		glEnableVertexAttribArray ( shader->AttributeTexture() );
		glVertexAttribPointer ( shader->AttributeNormal(), 2, GL_FLOAT, GL_FALSE, 0, 0 ); 
	}

	glDrawArrays( gltype, 0, vertexCount );
}

#endif