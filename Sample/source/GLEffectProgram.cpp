#include "stdafx.h"
#include "GLEffectProgram.h"

GLEffectProgram::GLEffectProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath) :
	GLProgram(commonShader, vertexShaderPath,fragmentShaderPath),
	cmatrix(glGetUniformLocation(programId,"vView"),glGetUniformLocation(programId,"MV"),glGetUniformLocation(programId,"MVP")),
	Fog(programId),
	Acid(glGetUniformLocation(programId,"uCurrentTime"),glGetUniformLocation(programId,"uAcidFactor")),
	Model(&cmatrix),
	Camera(&cmatrix)
	{


	uniformTint = glGetUniformLocation(programId,"uTint");
	attributeIndexTexture = glGetAttribLocation(programId,"vTex");
	attributeIndexVertex = glGetAttribLocation(programId,"vVert");
	attributeIndexColor = glGetAttribLocation(programId,"vColor");
};

void GLEffectProgram::SetTint(vec4 tint) {
	glUniform4f(uniformTint,tint.x,tint.y,tint.z,tint.w);
}

const GLint GLEffectProgram::AttributeTexture() {
	return attributeIndexTexture;
}
const GLint GLEffectProgram::AttributeVertex() {
	return attributeIndexVertex;
}
const GLint GLEffectProgram::AttributeColor() {
	return attributeIndexColor;
}