#include "stdafx.h"
#include "GLParticleProgram.h"

GLParticleProgram::GLParticleProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath) :
	GLProgram(commonShader, vertexShaderPath,fragmentShaderPath),
	Fog(programId),
	Acid(glGetUniformLocation(programId,"uCurrentTime"),glGetUniformLocation(programId,"uAcidFactor")) 
	{


	uniformView = glGetUniformLocation(programId,"uView");
	uniformProjection = glGetUniformLocation(programId,"uProjection");
	attributeIndexTexture = glGetAttribLocation(programId,"vTex");
	attributeIndexVertex = glGetAttribLocation(programId,"vVert");
	attributeIndexColor = glGetAttribLocation(programId,"vColor");
};

//Apply the given camera to this program
void GLParticleProgram::ApplyCamera(const GLCamera & camera) {
	mat4 view, projection;
	camera.CopyMatricies(&view,&projection);
	//Push this information unmultiplied to the vertex shader
	//Why unmultiplied? Because billboarding requires per-particle
	//modifications to the matrix before multiplication
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, (const GLfloat *)&view);
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, (const GLfloat *)&projection);

	vec3 test = vec3(1,5,3);
	mat4 otest = glm::translate(test);
	vec2 x = vec2(5,1);
}

const GLint GLParticleProgram::AttributeTexture() {
	return attributeIndexTexture;
}
const GLint GLParticleProgram::AttributeVertex() {
	return attributeIndexVertex;
}
const GLint GLParticleProgram::AttributeColor() {
	return attributeIndexColor;
}