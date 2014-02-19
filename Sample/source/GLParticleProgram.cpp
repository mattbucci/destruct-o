#include "stdafx.h"
#include "GLParticleProgram.h"

GLParticleProgram::GLParticleProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath) :
	GLProgram(commonShader, vertexShaderPath,fragmentShaderPath),
	Fog(programId),
	Acid(glGetUniformLocation(programId,"uCurrentTime"),glGetUniformLocation(programId,"uAcidFactor")) 
	{

	glUniform1i( glGetUniformLocation( programId, "basic_texture"), 0);

	uniformMVP = glGetUniformLocation(programId,"MVP");
	attributeIndexTexture = glGetAttribLocation(programId,"vTex");
	attributeIndexVertex = glGetAttribLocation(programId,"vVert");
};

//Apply the given camera to this program
void GLParticleProgram::ApplyCamera(const GLCamera & camera) {
	mat4 view, projection;
	camera.CopyMatricies(&view,&projection);
	//remove rotation from view matrix
	mat4 newView;
	//The upper left 3x3 (rotation/scaling) is identity
	newView[0].x = 1;
	newView[1].y = 1;
	newView[2].z = 1;
	//The right vector 4x1 (translation) is the same 
	newView[3] = view[3];
	//now combine with projection
	mat4 mvp = projection * newView;
	//And there's your MVP
	glUniformMatrix4fv(uniformMVP, 1, GL_FALSE, (const GLfloat *)&mvp);
}

const GLint GLParticleProgram::AttributeTexture() {
	return attributeIndexTexture;
}
const GLint GLParticleProgram::AttributeVertex() {
	return attributeIndexVertex;
}