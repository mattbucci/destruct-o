#pragma once

#include "GLProgram.h"
#include <map>

class ShaderGroup {
	map<string,GLProgram*> shaders;
public:
	ShaderGroup();
	~ShaderGroup();
	void AddShader(GLProgram * shader, string name);
	GLProgram * GetShader(string name);
};