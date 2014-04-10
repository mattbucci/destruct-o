#include "stdafx.h"
#include "GLMeshProgram.h"

// Get the mesh related shader properties
GLMeshProgram::GLMeshProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath)
    : GL3DProgram(commonShader, vertexShaderPath, fragmentShaderPath), uniformBones(glGetUniformLocation(programId, "bones"))
{
    attributeBoneWeights[0] = glGetAttribLocation(programId, "boneWeight0");
    attributeBoneWeights[1] = glGetAttribLocation(programId, "boneWeight1");
    attributeBoneWeights[2] = glGetAttribLocation(programId, "boneWeight2");
    attributeBoneWeights[3] = glGetAttribLocation(programId, "boneWeight3");
}

const GLint GLMeshProgram::AttributeBoneWeight(int idx)
{
    return attributeBoneWeights[idx];
}

const GLint GLMeshProgram::UniformBones()
{
    return uniformBones;
}