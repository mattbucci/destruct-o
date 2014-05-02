#include "stdafx.h"
#include "GLTerrainProgram.h"

GLTerrainProgram::GLTerrainProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath) :
	GLProgram(commonShader, vertexShaderPath,fragmentShaderPath),
	Model(glGetUniformLocation(programId, "M")),
	Camera(glGetUniformLocation(programId, "P"), glGetUniformLocation(programId, "V"), glGetUniformLocation(programId, "VP")),
	Fog(programId),
	Acid(glGetUniformLocation(programId,"uCurrentTime"),glGetUniformLocation(programId,"uAcidFactor")) 
{
	attributeIndexTexture = glGetAttribLocation(programId,"vTex");
	attributeIndexVertex = glGetAttribLocation(programId,"vVert");
	attributeIndexShading = glGetAttribLocation(programId,"vShading");
};

const GLint GLTerrainProgram::AttributeTexture() {
	return attributeIndexTexture;
}
const GLint GLTerrainProgram::AttributeVertex() {
	return attributeIndexVertex;
}
const GLint GLTerrainProgram::AttributeShading() {
	return attributeIndexShading;
}