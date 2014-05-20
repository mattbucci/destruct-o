
#pragma once
#include "stdafx.h"
#include "GLCamera.h"

class GLCommonShaderFile;
class GLShader;

class GLProgram {
protected:

	GLShader * vertexShader;
	GLShader * fragmentShader;
	GLShader * geometryShader;

	int programId;
public:
	GLProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath, string geometryShaderPath = "");
	virtual ~GLProgram();

	virtual	void UseProgram(void);

	void EnableTextureSlot(string uniformName,int slot);

	void SetUniform2f(vec2 val, string name);

	bool Valid(void);

	int GetId();
};