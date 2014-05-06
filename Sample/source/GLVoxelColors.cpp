#include "stdafx.h"
#include "GLVoxelColors.h"

//vec3[](vec3(0,1,.5), vec3(1,.75,0), vec3(.7,1,.9), vec3(.45,.45,0));


GLVoxelColors::GLVoxelColors(GLint voxelColorUniform) {
	this->voxelColorUniform = voxelColorUniform;
}

void GLVoxelColors::Apply() {
	//Voxel colors
	static const vec3 materialColors[] = {
		vec3(0,1,.5), 
		vec3(1,.75,0), 
		vec3(.7,1,.9), 
		vec3(.45,.45,0)
	};
	//Send all colors to shader
	glUniform3fv(voxelColorUniform,4,(float*)materialColors);
}