#include "stdafx.h"
#include "GLTerrainProgram.h"

GLTerrainProgram::GLTerrainProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath) :
	GLProgram(commonShader, vertexShaderPath,fragmentShaderPath),
	cmatrix(glGetUniformLocation(programId,"vView"),glGetUniformLocation(programId,"MV"),glGetUniformLocation(programId,"MVP")),
	Model(&cmatrix),
	Camera(&cmatrix),
	Fog(programId),
	Acid(glGetUniformLocation(programId,"uCurrentTime"),glGetUniformLocation(programId,"uAcidFactor")) 
	{

		glUniform1i( glGetUniformLocation( programId, "basic_texture"), 0);


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