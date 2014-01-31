#pragma once

#include "stdafx.h"
#include "GLLight.h"

class GLLighting {
	int activeLights;
	//All three lights
	GLLight a;
	GLLight b;
	GLLight c;
	//The count of activated lights
	GLint uniformLightCount;

	//Used to preserve light count during Off()
	int lastActivatedLightCount;
public:
	//Build GLLighting
	GLLighting(GLuint programId);
	~GLLighting();

	//Turn off lights, but perserve information
	//so that On() restores them as they were
	void Off();
	//Turn on lights that were previously turned off
	//Enable() can also be used
	void On();

	//Reset all three lighting effects, disable all three
	void Reset();

	//Enable the total number of lights specified. Supported 0-3 inclusive
	void Enable(int lightCount);

	//Apply current lighting effects to the shaders
	void Apply();

	//Turn off all previously enabled lighting effects
	//acts the same as Reset(); Apply();
	void Disable();

	//Select a light to modify its properties
	GLLight & operator[](const int lightId);
};