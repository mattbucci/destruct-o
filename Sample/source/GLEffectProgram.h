
#pragma once

#include "GLProgram.h"
#include "MaxDistanceFog.h"
#include "GLAcidTab.h"
#include "GLModel.h"


class GLEffectProgram : public GLProgram {
	GLint attributeIndexTexture;
	GLint attributeIndexVertex;
	GLint attributeIndexColor;

	GLint uniformTint;
	GLint uniformTexturesEnabled;
public:
	GLEffectProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath);

	MaxDistanceFog Fog;
	GLAcidTab Acid;
	GLModel Model;
	GLCamera Camera;
	
	void SetTint(vec4 tint);
	void UseTexture(bool textureEnabled);

	//All the attributes used for drawing special effects
	const GLint AttributeTexture();
	const GLint AttributeVertex();
	const GLint AttributeColor();
};