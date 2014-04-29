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

	attributeIndexTexture = glGetAttribLocation(programId,"vTex");
	attributeIndexPosition = glGetAttribLocation(programId,"vPos");
	attributeIndexVertex = glGetAttribLocation(programId,"vVert");
	attributeIndexNormal = glGetAttribLocation(programId,"vNorm");
    uniformMaterialReflectivity = glGetUniformLocation(programId, "material_reflectivity");
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
const GLint GL3DProgram::UniformMaterialReflectivity()
{
    return uniformMaterialReflectivity;
}