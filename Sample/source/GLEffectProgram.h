
#pragma once

#include "GLProgram.h"
#include "MaxDistanceFog.h"
#include "GLAcidTab.h"
#include "GLCombinedModel.h"
#include "GLCombinedMatrix.h"


class GLEffectProgram : public GLProgram {
	GLint attributeIndexTexture;
	GLint attributeIndexVertex;
	GLint attributeIndexColor;

	GLint uniformTint;
	GLCombinedMatrix cmatrix;
public:
	GLEffectProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath);

	MaxDistanceFog Fog;
	GLAcidTab Acid;
	GLCombinedModel Model;
	GLCamera Camera;
	
	void SetTint(vec4 tint);

	//All the attributes used for drawing special effects
	const GLint AttributeTexture();
	const GLint AttributeVertex();
	const GLint AttributeColor();
};