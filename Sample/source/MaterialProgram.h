/*
 *  Copyright 2014 Nathaniel Lewis
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef __MATERIAL_PROGRAM_H__
#define __MATERIAL_PROGRAM_H__

#include "GL3DProgram.h"
#include "Material.h"

class MaterialProgram : public GL3DProgram
{
    // The attribute locations for bone weights
    GLint attributeBoneWeights[4];
    
    // The attribute locations for texture coordinates
    GLint attributeTextures[2];
    
    // The uniform for the bones
    GLint uniformBones[20];
    
    // The texture maps
    GLint uniformTextures[4];
    
    // The uniform for whether or not the model is skinned
    GLint uniformSkinned;
    
    // The uniform for wheterh or not the model is textured
    GLint uniformTextured;
    
    // The uniform for the material colors
    GLint uniformColorAmbient;
    GLint uniformColorDiffuse;
    
public:
	MaterialProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath);
    
    // Get the attribute for a bone weight
	const GLint AttributeBoneWeight(int idx) const;
    
    // Get the attribute for a texture coordinate
    const GLint AttributeTexture(int idx) const;
    
    // Get the uniform for the skeleton
    const GLint UniformBones(int idx) const;
    
    // Get the uniform for the texture samplers
    const GLint UniformTexture(Material::TextureType idx) const;
    
    // Get the uniform for the skinning flag
    const GLint UniformSkinned() const;
    
    // Get the uniform for texturing
    const GLint UniformTextured() const;
    
    // Colors
    const GLint UniformColorAmbient() const;
    const GLint UniformColorDiffuse() const;
};

#endif
