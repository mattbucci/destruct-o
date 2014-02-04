#include "stdafx.h"
#include "GL2DProgram.h"


GL2DProgram::GL2DProgram(string vertexShaderPath, string fragmentShaderPath) : 
	GLProgram(vertexShaderPath,fragmentShaderPath),
	Model(glGetUniformLocation(programId,"uModel")),
	Clipping(glGetUniformLocation(programId,"clippingRegion")) {

	colorUniform = glGetUniformLocation(programId,"uColor");
	textureEnabled = glGetUniformLocation(programId,"textureEnabled");
	widthUniform = glGetUniformLocation(programId,"uWidth");
	heightUniform = glGetUniformLocation(programId,"uHeight");

	glBindAttribLocation(programId, 0, "vPos");
	glBindAttribLocation(programId, 1, "vTex");
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
	glUniform1f(textureEnabled,(enable) ? 1.0f : 0.0f);
}
