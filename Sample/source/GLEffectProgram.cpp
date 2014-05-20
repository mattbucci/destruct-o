#include "stdafx.h"
#include "GLEffectProgram.h"

GLEffectProgram::GLEffectProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath) :
	GLProgram(commonShader, vertexShaderPath,fragmentShaderPath),
	Fog(programId),
	Acid(glGetUniformLocation(programId,"uCurrentTime"),glGetUniformLocation(programId,"uAcidFactor")),
	Model(glGetUniformLocation(programId,"M")),
	Camera(glGetUniformLocation(programId,"P"),glGetUniformLocation(programId,"V"),glGetUniformLocation(programId,"VP"))
	{


	uniformTint = glGetUniformLocation(programId,"uTint");
	uniformTexturesEnabled = glGetUniformLocation(programId,"textureUsed");
	attributeIndexTexture = glGetAttribLocation(programId,"vTex");
	attributeIndexVertex = glGetAttribLocation(programId,"vVert");
	attributeIndexColor = glGetAttribLocation(programId,"vColor");
};

void GLEffectProgram::SetTint(vec4 tint) {
	glUniform4f(uniformTint,tint.x,tint.y,tint.z,tint.w);
}

void GLEffectProgram::UseTexture(bool textureEnabled) {
	glUniform1i(uniformTexturesEnabled,(textureEnabled) ? 5 : 0);
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