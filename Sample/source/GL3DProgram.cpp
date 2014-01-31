#include "stdafx.h"
#include "GL3DProgram.h"

GL3DProgram::GL3DProgram(string vertexShaderPath, string fragmentShaderPath) : 
	GLProgram(vertexShaderPath,fragmentShaderPath),
	Model(glGetUniformLocation(programId,"vModel")),
	Camera(glGetUniformLocation(programId,"vView"),glGetUniformLocation(programId,"vProj")),
	Lights(programId) {
};