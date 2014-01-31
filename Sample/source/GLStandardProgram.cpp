#include "stdafx.h"
#include "GLStandardProgram.h"

GLStandardProgram::GLStandardProgram(string vertexShader, string fragmentShader, string geometryShader) : 
	GLProgram(vertexShader,fragmentShader,geometryShader),
	Model(glGetUniformLocation(programId,"vModel")),
	Camera(glGetUniformLocation(programId,"vView"),glGetUniformLocation(programId,"vProj")) {

}
GLStandardProgram::~GLStandardProgram() {

}