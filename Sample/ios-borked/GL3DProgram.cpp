#include "stdafx.h"
#include "GL3DProgram.h"

GL3DProgram::GL3DProgram(string vertexShaderPath, string fragmentShaderPath) : 
	GLProgram(vertexShaderPath,fragmentShaderPath),
    Lights(programId),
	Model(glGetUniformLocation(programId,"vModel")),
	Camera(glGetUniformLocation(programId,"vView"),glGetUniformLocation(programId,"vProj")),
	Fog(programId){

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