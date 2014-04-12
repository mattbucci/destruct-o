#ifndef __GL_MESH_PROGRAM_H__
#define __GL_MESH_PROGRAM_H__

#include "GL3DProgram.h"
#include "Material.h"

class GLMeshProgram : public GL3DProgram
{
    // The attribute locations for bone weights
    GLint attributeBoneWeights[4];
    
    // The attribute locations for texture coordinates
    GLint attributeTextures[2];
    
    // The uniform for the bones
    GLint uniformBones;
    
    // The texture maps
    GLint uniformTextures[4];
    
public:
	GLMeshProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath);
    
    // Get the attribute for a bone weight
	const GLint AttributeBoneWeight(int idx);
    
    // Get the attribute for a texture coordinate
    const GLint AttributeTexture(int idx);
    
    // Get the uniform for the skeleton
    const GLint UniformBones();
    
    // Get the uniform for the texture samplers
    const GLint UniformTexture(Material::TextureType idx);
};

#endif
