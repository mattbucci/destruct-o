#include "stdafx.h"
#include "GL2DProgram.h"


GL2DProgram::GL2DProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath) : 
	GLProgram(commonShader,vertexShaderPath,fragmentShaderPath),
	Model(glGetUniformLocation(programId,"uModel")),
	Clipping(glGetUniformLocation(programId,"clippingRegion")) {

	colorUniform = glGetUniformLocation(programId,"uColor");
	textureEnabled = glGetUniformLocation(programId,"textureEnabled");
	widthUniform = glGetUniformLocation(programId,"uWidth");
	heightUniform = glGetUniformLocation(programId,"uHeight");

	attributeIndexTexture = glGetAttribLocation(programId,"vTex");
	attributeIndexPosition = glGetAttribLocation(programId,"vPos");
}

const GLint GL2DProgram::AttributePosition() {
	return attributeIndexPosition;
}
const GLint GL2DProgram::AttributeTexture() {
	return attributeIndexTexture;
}

void GL2DProgram::SetColor(vec4 color) {
	glUniform4f(colorUniform,color.x,color.y,color.z,color.w);
}

void GL2DProgram::SetWidthHeight(float width, float height) {
	glUniform1f(widthUniform,width);
	glUniform1f(heightUniform,height);
}

void GL2DProgram::EnableTexture(bool enable) {
	//A hack to force correct texture (shouldn't be necessary?)
	glUniform1i( glGetUniformLocation( programId, "basic_texture"), 0);
	//Enable the texture in the shader
	glUniform1f(textureEnabled,(enable) ? 1.0f : 0.0f);
}
