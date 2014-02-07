#include "stdafx.h"
#include "InstancedVoxelRenderSystem.h"
#include "GL3DProgram.h"

#ifndef __ANDROID__

InstancedVoxelRenderSystem::InstancedVoxelRenderSystem() {
	glGenBuffers(1,&vertexBuffer);
	glGenBuffers(1,&textureBuffer);
	glGenBuffers(1,&normalBuffer);
	glGenBuffers(1,&positionBuffer);
    glGenBuffers(1,&indexBuffer);
    
// Different vertex array generation for iOS
#ifdef __IPHONEOS__
    glGenVertexArraysOES(1, &vertexArray);
#else
	glGenVertexArrays(1,&vertexArray);
#endif
    
	bufferedVoxels = 0;
	vertices = new vec3[36];
	textureCoordinates = new vec2[36];
	normals = new vec3[36];
	positions = new vec4[INSTANCE_RENDER_SWEEP];

	//Build the cube just this once
	int vertNumber = 0;

	//A cube

	//Top
	pushSide(vec3(0,0,1),vec3(0,0,1),vec3(0,1,1),vec3(1,0,1),vec3(1,1,1),vertNumber);
	//Bottom
	pushSide(vec3(0,0,-1),vec3(0,0,0),vec3(0,1,0),vec3(1,0,0),vec3(1,1,0),vertNumber);
	//Left
	pushSide(vec3(1,0,0),vec3(1,0,0),vec3(1,1,0),vec3(1,0,1),vec3(1,1,1),vertNumber);
	//Right
	pushSide(vec3(-1,0,0),vec3(0,0,0),vec3(0,1,0),vec3(0,0,1),vec3(0,1,1),vertNumber);
	//Back
	pushSide(vec3(0,1,0),vec3(0,1,0),vec3(0,1,1),vec3(1,1,0),vec3(1,1,1),vertNumber);
	//Front
	pushSide(vec3(0,-1,0),vec3(0,0,0),vec3(1,0,0),vec3(0,0,1),vec3(1,0,1),vertNumber);

	//Generate the sequential indices
	for (int i = 0; i < 36; i++)
		indices.push_back(i);

	allocated = false;
}

InstancedVoxelRenderSystem::~InstancedVoxelRenderSystem() {
	delete [] vertices;
	delete [] textureCoordinates;
	delete [] normals;
	delete [] positions;

	glDeleteBuffers(1,&vertexBuffer);
	glDeleteBuffers(1,&textureBuffer);
	glDeleteBuffers(1,&normalBuffer);
	glDeleteBuffers(1,&positionBuffer);
    
    // Different vertex array deletion for iOS
#ifdef __IPHONEOS__
	glDeleteVertexArraysOES(1,&vertexArray);
#else
	glDeleteVertexArrays(1,&vertexArray);
#endif
    
}

void InstancedVoxelRenderSystem::pushSide(vec3 normal, vec3 a, vec3 b, vec3 c, vec3 d, int & vertNumber) {
	normals[vertNumber] = normal;
	textureCoordinates[vertNumber] = vec2(0,0);
	vertices[vertNumber++] = a;
	normals[vertNumber] = normal;
	textureCoordinates[vertNumber] = vec2(1,0);
	vertices[vertNumber++] = b;
	normals[vertNumber] = normal;
	textureCoordinates[vertNumber] = vec2(0,1);
	vertices[vertNumber++] = c;
	normals[vertNumber] = normal;
	textureCoordinates[vertNumber] = vec2(1,0);
	vertices[vertNumber++] = b;
	normals[vertNumber] = normal;
	textureCoordinates[vertNumber] = vec2(1,1);
	vertices[vertNumber++] = d;
	normals[vertNumber] = normal;
	textureCoordinates[vertNumber] = vec2(0,1);
	vertices[vertNumber++] = c;
}

//Called at the start of the draw cycle
//pushes the cube vertices/normals/texture coordinates to the gpu
void InstancedVoxelRenderSystem::startDraw(GL3DProgram * shader) {
	//Only run gpu allocation code once
	if (allocated)
		return;

	allocated = true;

	//Allocate the space for the gpu buffers now
	//and send the static data
	//Rebind the array to bring them into the current context
    
    // Different vertex array binding for iOS
#ifdef __IPHONEOS__
	glBindVertexArrayOES(vertexArray);
#else
	glBindVertexArray ( vertexArray );
#endif

	//Push voxel to gpu
	glBindBuffer ( GL_ARRAY_BUFFER, vertexBuffer );
	glBufferData ( GL_ARRAY_BUFFER, 36*sizeof(vec3), vertices, GL_STATIC_DRAW );
	glEnableVertexAttribArray ( shader->AttributeVertex() );
	glVertexAttribPointer ( shader->AttributeVertex(), 3, GL_FLOAT, GL_FALSE, 0, 0 );

	glBindBuffer ( GL_ARRAY_BUFFER, textureBuffer );
	glBufferData ( GL_ARRAY_BUFFER, 36*sizeof(vec2), textureCoordinates, GL_STATIC_DRAW );
	glEnableVertexAttribArray ( shader->AttributeTexture() );
	glVertexAttribPointer ( shader->AttributeTexture(), 2, GL_FLOAT, GL_FALSE, 0, 0 );
	
	glBindBuffer ( GL_ARRAY_BUFFER, normalBuffer );
	glBufferData ( GL_ARRAY_BUFFER, 36*sizeof(vec3), normals, GL_STATIC_DRAW );
	glEnableVertexAttribArray ( shader->AttributeNormal() );
	glVertexAttribPointer ( shader->AttributeNormal(), 3, GL_FLOAT, GL_FALSE, 0, 0 );
    
	//Allocate space for positions
	glBindBuffer ( GL_ARRAY_BUFFER, positionBuffer );
	glBufferData ( GL_ARRAY_BUFFER, INSTANCE_RENDER_SWEEP*sizeof(vec4), positions, GL_DYNAMIC_DRAW );
	glEnableVertexAttribArray ( shader->AttributePosition() );
	glVertexAttribPointer ( shader->AttributePosition(), 4, GL_FLOAT, GL_FALSE, 0, 0 );
    
    //Push indices (not necessary on windows... for some reason?
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,36,&indices[0],GL_STATIC_DRAW);
}

void InstancedVoxelRenderSystem::draw(GL3DProgram * shader) {

    // Different vertex array binding for iOS
#ifdef __IPHONEOS__
	glBindVertexArrayOES(vertexArray);
#else
	glBindVertexArray ( vertexArray );
#endif
	glBindBuffer ( GL_ARRAY_BUFFER, positionBuffer );
	glBufferSubData ( GL_ARRAY_BUFFER, 0,INSTANCE_RENDER_SWEEP*sizeof(vec4), positions );
	glEnableVertexAttribArray ( shader->AttributePosition() );
	glVertexAttribPointer ( shader->AttributePosition(), 4, GL_FLOAT, GL_FALSE, 0, 0 );
    
    // Different instancing operations for iOS
#ifdef __IPHONEOS__
	//The position is per-instance
	//everything else is per-vertex
	glVertexAttribDivisorEXT(shader->AttributeNormal(),0);
	glVertexAttribDivisorEXT(shader->AttributePosition(),1);
	glVertexAttribDivisorEXT(shader->AttributeTexture(),0);
	glVertexAttribDivisorEXT(shader->AttributeVertex(),0);
    
	glDrawElementsInstancedEXT(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0, bufferedVoxels);
#else
	//The position is per-instance
	//everything else is per-vertex
	glVertexAttribDivisor(shader->AttributeNormal(),0);
	glVertexAttribDivisor(shader->AttributePosition(),1);
	glVertexAttribDivisor(shader->AttributeTexture(),0);
	glVertexAttribDivisor(shader->AttributeVertex(),0);
    
	glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0, bufferedVoxels);
#endif

	//All buffered voxels now drawn
	bufferedVoxels = 0;
}

//Add a voxel and maybe draw
//Called per voxel
void InstancedVoxelRenderSystem::pushVoxel(GL3DProgram * shader, vec3 pos,int materialId) {
	positions[bufferedVoxels++] = vec4(pos,(float)materialId);

	if (bufferedVoxels == INSTANCE_RENDER_SWEEP) {
		draw(shader);
	}
}

//Called at the end of the draw cycle
//draws any undrawn voxels
void InstancedVoxelRenderSystem::finishDraw(GL3DProgram * shader) {
	if (bufferedVoxels > 0)
		draw(shader);
}

#endif