#include "stdafx.h"
#include "BasicVoxelRenderSystem.h"
#include "GL3DProgram.h"

//This voxel render system uses glsl 110 but still
//needs to make a special case for android

BasicVoxelRenderSystem::BasicVoxelRenderSystem() {
#ifndef __ANDROID__
	glGenBuffers(1,&vertexBuffer);
	glGenVertexArrays(1,&vertexArray);
#endif
	bufferedVoxels = 0;
	vertices = new vec4[36*VOXEL_RENDER_SWEEP];

	allocated = false;
}

BasicVoxelRenderSystem::~BasicVoxelRenderSystem() {
	delete [] vertices;
#ifndef __ANDROID__
	glDeleteBuffers(1,&vertexBuffer);
	glDeleteVertexArrays(1,&vertexArray);
#endif
}

void BasicVoxelRenderSystem::pushSide(vec3 pos, vec3 normal, vec3 a, vec3 b, vec3 c, vec3 d, int & vertNumber,int materialId) {
	int curVoxel = bufferedVoxels*36;
    float voxelNormal = (float)vertNumber/6.0+.5 + (float)materialId*100.0f;
	//The 4th float in each vector is a vertex id mixed with the material id
	//and is used to generate normals
    //Texture coordinates are encoded in the negative signs of .x and .y
	vertices[curVoxel+vertNumber++] = vec4(a+pos,voxelNormal)*vec4(1,1,1,1);
	vertices[curVoxel+vertNumber++] = vec4(b+pos,voxelNormal)*vec4(-1,1,1,1);
	vertices[curVoxel+vertNumber++] = vec4(c+pos,voxelNormal)*vec4(1,-1,1,1);
	vertices[curVoxel+vertNumber++] = vec4(b+pos,voxelNormal)*vec4(-1,1,1,1);
	vertices[curVoxel+vertNumber++] = vec4(d+pos,voxelNormal)*vec4(-1,-1,1,1);
	vertices[curVoxel+vertNumber++] = vec4(c+pos,voxelNormal)*vec4(1,-1,1,1);
}

//Called at the start of the draw cycle
//pushes the cube vertices/normals/texture coordinates to the gpu
void BasicVoxelRenderSystem::startDraw(GL3DProgram * shader) {
	//Only run gpu allocation code once
	if (allocated)
		return;

	allocated = true;
	glEnableVertexAttribArray ( 0 );
#ifndef __ANDROID__
	//Allocate the space for the gpu buffers now
	//and send the static data
	//Rebind the array to bring them into the current context
	glBindVertexArray ( vertexArray );

	//Allocate space
	glBindBuffer ( GL_ARRAY_BUFFER, vertexBuffer );
	glBufferData ( GL_ARRAY_BUFFER, VOXEL_RENDER_SWEEP*36*sizeof(vec4), vertices, GL_DYNAMIC_DRAW );
	glEnableVertexAttribArray ( shader->AttributePosition() );
	glVertexAttribPointer ( shader->AttributePosition(), 4, GL_FLOAT, GL_FALSE, 0, 0 );
#endif
}

void BasicVoxelRenderSystem::draw(GL3DProgram * shader) {
	glEnableVertexAttribArray ( 0 );
#ifndef __ANDROID__
	glBindVertexArray ( vertexArray );
	glBindBuffer ( GL_ARRAY_BUFFER, vertexBuffer );
	glBufferSubData ( GL_ARRAY_BUFFER, 0,bufferedVoxels*36*sizeof(vec4), vertices );
	glEnableVertexAttribArray ( shader->AttributePosition() );
	glVertexAttribPointer ( shader->AttributePosition(), 4, GL_FLOAT, GL_FALSE, 0, 0 );	
#else
	glVertexAttribPointer ( shader->AttributePosition(), 4, GL_FLOAT, GL_FALSE, 0, vertices );	
#endif

	glDrawArrays( GL_TRIANGLES, 0, bufferedVoxels*36 );
	//All buffered voxels now drawn
	bufferedVoxels = 0;
}

//Add a voxel and maybe draw
//Called per voxel
void BasicVoxelRenderSystem::pushVoxel(GL3DProgram * shader, vec3 pos,int materialId) {
	//So the original plan was to use one buffer that's why we have vertNumber
	//but I ran out of time
	int vertNumber = 0;

	//A cube
	

	//Top
	pushSide(pos,vec3(0,0,1),vec3(0,0,1),vec3(0,1,1),vec3(1,0,1),vec3(1,1,1),vertNumber,materialId);
	//Bottom
	pushSide(pos,vec3(0,0,-1),vec3(0,0,0),vec3(0,1,0),vec3(1,0,0),vec3(1,1,0),vertNumber,materialId);
	//Left
	pushSide(pos,vec3(1,0,0),vec3(1,0,0),vec3(1,1,0),vec3(1,0,1),vec3(1,1,1),vertNumber,materialId);
	//Right
	pushSide(pos,vec3(-1,0,0),vec3(0,0,0),vec3(0,1,0),vec3(0,0,1),vec3(0,1,1),vertNumber,materialId);
	//Back
	pushSide(pos,vec3(0,1,0),vec3(0,1,0),vec3(0,1,1),vec3(1,1,0),vec3(1,1,1),vertNumber,materialId);
	//Front
	pushSide(pos,vec3(0,-1,0),vec3(0,0,0),vec3(1,0,0),vec3(0,0,1),vec3(1,0,1),vertNumber,materialId);

	bufferedVoxels++;
	if (bufferedVoxels == VOXEL_RENDER_SWEEP) {
		draw(shader);
	}
}

//Called at the end of the draw cycle
//draws any undrawn voxels
void BasicVoxelRenderSystem::finishDraw(GL3DProgram * shader) {
	if (bufferedVoxels > 0)
		draw(shader);
}
