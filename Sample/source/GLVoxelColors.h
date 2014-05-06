
#pragma once

#include "stdafx.h"

class GLVoxelColors {
	GLint voxelColorUniform;
public:
	GLVoxelColors(GLint voxelColorUniform);

	void Apply();
};