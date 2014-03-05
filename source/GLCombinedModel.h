#pragma once


#pragma once

#include "stdafx.h"

class GLCombinedMatrix;

class GLCombinedModel {
	mat4 modelMatrix[256];
	int stackPosition;
	GLCombinedMatrix * cmatrix;
public:
	GLCombinedModel(GLCombinedMatrix * cmatrix);

	//Destroys matrix stack and sets matrix to identity
	void Reset();
	//Applies the current matrix to the 3d shader
	void Apply();

	mat4 GetMatrix(void);
	void SetMatrix(const mat4 & matrix);

	vec3 TransformVector(vec3 vertex) const;

	void PushMatrix(void);
	void PopMatrix(void);

	GLCombinedModel & Translate(vec3 translation);
	GLCombinedModel & Translate(float x, float y, float z);
	GLCombinedModel & Rotate(float angle, vec3 axis);
	GLCombinedModel & Rotate(float angle, float axisX, float axisY, float axisZ);
	GLCombinedModel & Scale(vec3 scale);
	GLCombinedModel & Scale(float xScale, float yScale, float zScale);
	GLCombinedModel & Combine(mat4 matrix);
	
	//Extra handy functions
	GLCombinedModel & Project(vec3 direction, vec3 pointOnPlane, vec3 planeNormal);
};