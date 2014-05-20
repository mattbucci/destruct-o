
#pragma once
#include "GLProgram.h"
#include "MaxDistanceFog.h"
#include "GLAcidTab.h"

class GLParticleProgram : public GLProgram {
	GLint attributeIndexTexture;
	GLint attributeIndexVertex;
	GLint attributeIndexColor;
	GLint attributeIndexVertexNumber;

	GLint uniformView;
	GLint uniformProjection;

public:
	GLParticleProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath);

	//Apply the given camera to this program
	void ApplyCamera(const GLCamera & camera);

	//These have to be implemented but are not currently
	MaxDistanceFog Fog;
	GLAcidTab Acid;

	//All the attributes used for drawing particles
	const GLint AttributeTexture();
	const GLint AttributeVertex();
	const GLint AttributeColor();
	const GLint AttributeVertexNumber();
};