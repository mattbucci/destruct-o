#include "stdafx.h"
#include "GLVoxelColors.h"

//Voxel colors
const vec3 GLVoxelColors::MaterialColors[] = {
	vec3(0,1,.5), 
	vec3(1,.75,0), 
	vec3(.7,1,.9), 
	vec3(.45,.45,0)
};

GLVoxelColors::GLVoxelColors(GLint voxelColorUniform) {
	this->voxelColorUniform = voxelColorUniform;
}

void GLVoxelColors::Apply() {

	//Send all colors to shader
	glUniform3fv(voxelColorUniform,4,(float*)MaterialColors);
}