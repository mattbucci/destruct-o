#include "stdafx.h"
#include "GLCombinedMatrix.h"


GLCombinedMatrix::GLCombinedMatrix(GLint viewMatrix, GLint modelViewMatrix, GLint modelViewProjectMatrix) {
	uniformViewMatrix = viewMatrix;
	uniformModelViewMatrix = modelViewMatrix;
	uniformModelViewProjectMatrix = modelViewProjectMatrix;
}
GLCombinedMatrix::~GLCombinedMatrix() {

}

//Precalculated and apply the MVP matrices
void GLCombinedMatrix::Apply() {
	// Use precalculated modelview and modelviewprojection matricies
	// Calculate matricies
	mat4 modelViewMatrix = View * Model;
	mat4 modelViewProjectionMatrix = Projection * modelViewMatrix;
	
	// Assign to shader
	glUniformMatrix4fv(uniformViewMatrix, 1, GL_FALSE, (const GLfloat *)&modelViewMatrix);
	glUniformMatrix4fv(uniformModelViewMatrix, 1, GL_FALSE, (const GLfloat *)&modelViewMatrix);
	glUniformMatrix4fv(uniformModelViewProjectMatrix, 1, GL_FALSE, (const GLfloat *)&modelViewProjectionMatrix);
}