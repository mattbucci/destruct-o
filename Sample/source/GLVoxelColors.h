
#pragma once

#include "stdafx.h"

class GLVoxelColors {
	GLint voxelColorUniform;
public:
	static const vec3 MaterialColors[];

	GLVoxelColors(GLint voxelColorUniform);

	void Apply();
};