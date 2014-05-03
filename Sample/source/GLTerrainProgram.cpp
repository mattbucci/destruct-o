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
    uniformMatrixModelView = glGetUniformLocation(programId, "MV");
    uniformMatrixModelViewProjection = glGetUniformLocation(programId, "MVP");
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

void GLTerrainProgram::Apply()
{
    // Work in progress matrix
    mat4 storage = Model.GetMatrix();
    
    // If we have a model view matrix
    if(uniformMatrixModelView >= 0)
    {
        storage = Camera.ViewMatrix() * storage;
        glUniformMatrix4fv(uniformMatrixModelView, 1, GL_FALSE, &storage[0][0]);
    }
    
    // If we have a model view projection matrix
    if(uniformMatrixModelViewProjection >= 0)
    {
        // If the matrix was already premultiplied
        if(uniformMatrixModelView >= 0)
        {
            storage = Camera.ProjectionMatrix() * storage;
        } else
        {
            // Do the full chain
            storage = Camera.ProjectionMatrix() * Camera.ViewMatrix() * storage;
        }
        
        // Set the mvp matrix
        glUniformMatrix4fv(uniformMatrixModelViewProjection, 1, GL_FALSE, &storage[0][0]);
    }
}