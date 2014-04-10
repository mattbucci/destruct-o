#ifndef __GL_MESH_PROGRAM_H__
#define __GL_MESH_PROGRAM_H__

#include "GL3DProgram.h"

class GLMeshProgram : public GL3DProgram
{
    // The locations for mesh specific properties
    GLint attributeBoneWeights[4];
    
    // The uniform for the bones
    GLint uniformBones;
    
public:
	GLMeshProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath);
    
    // Get the attribute for a bone weight
	const GLint AttributeBoneWeight(int idx);
    
    // Get the uniform for the skeleton
    const GLint UniformBones();
};

#endif
