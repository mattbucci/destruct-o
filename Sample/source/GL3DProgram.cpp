#include "stdafx.h"
#include "GL3DProgram.h"

GL3DProgram::GL3DProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath) :
	GLProgram(commonShader, vertexShaderPath,fragmentShaderPath),
	Lights(programId),
	Model(glGetUniformLocation(programId,"M")),
	Camera(glGetUniformLocation(programId,"P"),glGetUniformLocation(programId,"V"),glGetUniformLocation(programId,"VP")),
	Fog(programId),
	Acid(glGetUniformLocation(programId,"uCurrentTime"),glGetUniformLocation(programId,"uAcidFactor")) 
{

	attributeIndexTexture = glGetAttribLocation(programId,"vTex");
	attributeIndexPosition = glGetAttribLocation(programId,"vPos");
	attributeIndexVertex = glGetAttribLocation(programId,"vVert");
	attributeIndexNormal = glGetAttribLocation(programId,"vNorm");
    uniformMaterialReflectivity = glGetUniformLocation(programId, "material_reflectivity");
    uniformMatrixModelView = glGetUniformLocation(programId, "MV");
    uniformMatrixModelViewProjection = glGetUniformLocation(programId, "MVP");
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

void GL3DProgram::Apply()
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
