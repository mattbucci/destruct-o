
#pragma once

#include "stdafx.h"

class GLCombinedMatrix {
	GLint uniformViewMatrix;
	GLint uniformModelViewMatrix;
	GLint uniformModelViewProjectMatrix;
public:
	GLCombinedMatrix(GLint viewMatrix, GLint modelViewMatrix, GLint modelViewProjectMatrix);
	~GLCombinedMatrix();

	mat4 Model;
	mat4 View;
	mat4 Projection;

	//Precalculated and apply the MVP matrices
	void Apply();
};