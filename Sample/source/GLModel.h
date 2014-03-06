
#pragma once

#include "stdafx.h"

class GLModel {
	GLint model;
	mat4 modelMatrix[256];
	int stackPosition;
public:
	GLModel(GLint cameraUniformId);

	//Destroys matrix stack and sets matrix to identity
	void Reset();
	//Applies the current matrix to the 3d shader
	void Apply();

	mat4 GetMatrix(void);
	void SetMatrix(const mat4 & matrix);

	vec3 TransformVector(vec3 vertex) const;

	void PushMatrix(void);
	void PopMatrix(void);

	GLModel & Translate(vec3 translation);
	GLModel & Translate(float x, float y, float z);
	GLModel & Rotate(float angle, vec3 axis);
	GLModel & Rotate(float angle, float axisX, float axisY, float axisZ);
	GLModel & Scale(vec3 scale);
	GLModel & Scale(float xScale, float yScale, float zScale);
	
	//Extra handy functions
	GLModel & Project(vec3 direction, vec3 pointOnPlane, vec3 planeNormal);
};