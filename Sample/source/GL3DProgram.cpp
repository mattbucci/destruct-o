#include "stdafx.h"
#include "GL3DProgram.h"

GL3DProgram::GL3DProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath) :
	GLProgram(commonShader, vertexShaderPath,fragmentShaderPath),
	cmatrix(glGetUniformLocation(programId,"vView"),glGetUniformLocation(programId,"MV"),glGetUniformLocation(programId,"MVP")),
	Lights(programId),
	Model(&cmatrix),
	Camera(&cmatrix),
	Fog(programId),
	Acid(glGetUniformLocation(programId,"uCurrentTime"),glGetUniformLocation(programId,"uAcidFactor")) 
	{

		glUniform1i( glGetUniformLocation( programId, "basic_texture"), 0);


	attributeIndexTexture = glGetAttribLocation(programId,"vTex");
	attributeIndexPosition = glGetAttribLocation(programId,"vPos");
	attributeIndexVertex = glGetAttribLocation(programId,"vVert");
	attributeIndexNormal = glGetAttribLocation(programId,"vNorm");
};

const GLint GL3DProgram::AttributePosition() {
	return attributeIndexPosition;
}
const GLint GL3DProgram::AttributeTexture() {
	return attributeIndexTexture;
}
const GLint GL3DProgram::AttributeVertex() {
	return attributeIndexVertex;
}
const GLint GL3DProgram::AttributeNormal() {
	return attributeIndexNormal;
}