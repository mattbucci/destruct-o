#include "stdafx.h"
#include "InstancedVoxelRenderSystem.h"
#include "GL3DProgram.h"


InstancedVoxelRenderSystem::InstancedVoxelRenderSystem() {
	glGenBuffers(1,&vertexBuffer);
	glGenBuffers(1,&textureBuffer);
	glGenBuffers(1,&normalBuffer);
	glGenBuffers(1,&positionBuffer);
	glGenBuffers(1,&indexBuffer);
	
	glGenVertexArrays(1,&vertexArray);
	
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
	
	glDeleteVertexArrays(1,&vertexArray);
}

void InstancedVoxelRenderSystem::pushSide(vec3 normal, vec3 a, vec3 b, vec3 c, vec3 d, int & vertNumber) {
	const float texturePixel = 0.0f;
	normals[vertNumber] = normal;
	textureCoordinates[vertNumber] = vec2(0.05,0.05);
	vertices[vertNumber++] = a;
	normals[vertNumber] = normal;
	textureCoordinates[vertNumber] = vec2(.95,0.05);
	vertices[vertNumber++] = b;
	normals[vertNumber] = normal;
	textureCoordinates[vertNumber] = vec2(0.05,.95);
	vertices[vertNumber++] = c;
	normals[vertNumber] = normal;
	textureCoordinates[vertNumber] = vec2(.95,0.05);
	vertices[vertNumber++] = b;
	normals[vertNumber] = normal;
	textureCoordinates[vertNumber] = vec2(.95,.95);
	vertices[vertNumber++] = d;
	normals[vertNumber] = normal;
	textureCoordinates[vertNumber] = vec2(.05,.95);
	vertices[vertNumber++] = c;
}

//Called at the start of the draw cycle
//pushes the cube vertices/normals/texture coordinates to the gpu
void InstancedVoxelRenderSystem::startDraw(GL3DProgram * shader) {
	//Only run gpu allocation code once
	if (allocated)
		return;

	allocated = true;

	cout << "!!<>!! Successfully instanced a voxel instance system\n";

	//Allocate the space for the gpu buffers now
	//and send the static data
	//Rebind the array to bring them into the current context

	glBindVertexArray ( vertexArray );

	//Push voxel to gpu
	glBindBuffer ( GL_ARRAY_BUFFER, vertexBuffer );
	glBufferData ( GL_ARRAY_BUFFER, 36*sizeof(vec3), vertices, GL_STATIC_DRAW );
	glEnableVertexAttribArray ( shader->AttributeVertex() );
	glVertexAttribPointer ( shader->AttributeVertex(), 3, GL_FLOAT, GL_FALSE, 0, 0 );
	glVertexAttribDivisor(shader->AttributeVertex(),0);

	glBindBuffer ( GL_ARRAY_BUFFER, textureBuffer );
	glBufferData ( GL_ARRAY_BUFFER, 36*sizeof(vec2), textureCoordinates, GL_STATIC_DRAW );
	glEnableVertexAttribArray ( shader->AttributeTexture() );
	glVertexAttribPointer ( shader->AttributeTexture(), 2, GL_FLOAT, GL_FALSE, 0, 0 );
	glVertexAttribDivisor(shader->AttributeTexture(),0);
	
	glBindBuffer ( GL_ARRAY_BUFFER, normalBuffer );
	glBufferData ( GL_ARRAY_BUFFER, 36*sizeof(vec3), normals, GL_STATIC_DRAW );
	glEnableVertexAttribArray ( shader->AttributeNormal() );
	glVertexAttribPointer ( shader->AttributeNormal(), 3, GL_FLOAT, GL_FALSE, 0, 0 );
	glVertexAttribDivisor(shader->AttributeNormal(),0);
	
	//Allocate space for positions
	glBindBuffer ( GL_ARRAY_BUFFER, positionBuffer );
	glBufferData ( GL_ARRAY_BUFFER, INSTANCE_RENDER_SWEEP*sizeof(vec4), positions, GL_DYNAMIC_DRAW );
	glEnableVertexAttribArray ( shader->AttributePosition() );
	glVertexAttribPointer ( shader->AttributePosition(), 4, GL_FLOAT, GL_FALSE, 0, 0 );
	glVertexAttribDivisor(shader->AttributePosition(),1);
	
	//Push indices (not necessary on windows... for some reason?
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,36,&indices[0],GL_STATIC_DRAW);
}

void InstancedVoxelRenderSystem::draw(GL3DProgram * shader)
{
	glBindVertexArray ( vertexArray );

	glBindBuffer ( GL_ARRAY_BUFFER, positionBuffer );
	glBufferSubData ( GL_ARRAY_BUFFER, 0,bufferedVoxels*sizeof(vec4), positions );
	
	glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0, bufferedVoxels);

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

