
#pragma once

#include "stdafx.h"

class MaxDistanceFog {
	vec4 color;

	float fogDistance;
	vec4 fogColor;

	int glFogDistance;
	int glFogColor;
public:
	MaxDistanceFog(GLint programId);
	~MaxDistanceFog();

	//Also set the clear color
	//to create a matching effect
	void SetFogColor(vec4 color);
	vec4 GetFogColor();

	//Set fog distance
	//fog starts to appear 3/4ths into this distance
	void SetFogDistance(float distance);
	float GetFogDistance();
};