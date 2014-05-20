#pragma once

#include "GLProgram.h"

#include "GLModel.h"
#include "GLClipping.h"

class GL2DProgram : public GLProgram {
	GLint colorUniform;
	GLint textureEnabled;
	GLint widthUniform;
	GLint heightUniform;
	GLint attributeIndexPosition;
	GLint attributeIndexTexture;
public:
	GL2DProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath);

	GLModel Model;
	GLClipping Clipping;

	void SetWidthHeight(float width, float height);
	void SetColor(vec4 color);

	//Also binds attributes
	//vertex to 0
	//texture to 1
	//void UseProgram() override;

	const GLint AttributePosition();
	const GLint AttributeTexture();

	//For manually bound textures
	void EnableTexture(bool enable);
};