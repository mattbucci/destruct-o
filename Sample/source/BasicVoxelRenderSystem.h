
#pragma once

#include "VoxelDrawSystem.h"

//How many voxels to render max at one time
#define VOXEL_RENDER_SWEEP 200
class BasicVoxelRenderSystem : public VoxelDrawSystem {
	//Push a side of the cube
	//Only used during initial construction of the cube during the construction of the instanced render system
	void pushSide(vec3 pos, vec3 normal, vec3 a, vec3 b, vec3 c, vec3 d, int & vertNumber,int materialId);

	//Opengl buffers
	GLuint vertexArray;
	GLuint vertexBuffer;

	//The cpu-side buffers
	vec4 * vertices;
	//Is false until the gpu buffer has been
	//allocated during the first draw cycle
	bool allocated;

	int bufferedVoxels;

	//Flushes the CPU side buffer to the gpu using instanced rendering
	void draw(GL3DProgram * shader);
public:
	BasicVoxelRenderSystem();
	~BasicVoxelRenderSystem();

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

