#pragma once

#include "GLProgram.h"

#include "GLModel.h"
#include "GLClipping.h"

class GL2DProgram : public GLProgram {
	GLint colorUniform;
	GLint textureEnabled;
	GLint widthUniform;
	GLint heightUniform;
public:
	GL2DProgram(string vertexShaderPath, string fragmentShaderPath);

	GLModel Model;
	GLClipping Clipping;

	void SetWidthHeight(float width, float height);
	void SetColor(vec4 color);

	//For manually bound textures
	void EnableTexture(bool enable);
};