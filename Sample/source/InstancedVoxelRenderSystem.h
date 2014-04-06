
#pragma once

#include "VoxelDrawSystem.h"


//Defines how many instances will be rendered (max) per
//draw call
#define INSTANCE_RENDER_SWEEP 50000
class InstancedVoxelRenderSystem : public VoxelDrawSystem {
	//Push a side of the cube
	//Only used during initial construction of the cube during the construction of the instanced render system
	void pushSide(vec3 normal, vec3 a, vec3 b, vec3 c, vec3 d, int & vertNumber);

	//Opengl buffers
	GLuint vertexArray;
	GLuint vertexBuffer;
	GLuint textureBuffer;
	GLuint normalBuffer;
    GLuint indexBuffer;
	//Used for instanced positions
	GLuint positionBuffer;

	//The cpu-side buffers
	vec3 * vertices;
	vec2 * textureCoordinates;
	vec3 * normals;
	vec4 * positions; //the 4th float is the material id
	vector<GLubyte> indices;
	//How many voxels are in the buffers right now
	int bufferedVoxels;
	//Is false until the gpu buffers have been
	//allocated during the first draw cycle
	bool allocated;

	//The number of vertices per voxel
	int vertexCount;

	//Flushes the CPU side buffer to the gpu using instanced rendering
	void draw(GL3DProgram * shader);
public:
	InstancedVoxelRenderSystem(bool drawBottoms = false);
	~InstancedVoxelRenderSystem();

	//Called at the start of the draw cycle
	//does nothing in this render system
	void startDraw(GL3DProgram * shader) override;

	//Add a voxel and maybe draw
	//Called per voxel
	void pushVoxel(GL3DProgram * shader, vec3 pos,int materialId) override;

	//Called at the end of the draw cycle
	//draws any undrawn voxels
	void finishDraw(GL3DProgram * shader) override;
};

