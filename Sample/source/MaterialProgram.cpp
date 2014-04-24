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

#include "stdafx.h"
#include "MaterialProgram.h"
#include <sstream>

// Get the mesh related shader properties
MaterialProgram::MaterialProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath)
    : GL3DProgram(commonShader, vertexShaderPath, fragmentShaderPath)
{
    attributeBoneWeights[0] = glGetAttribLocation(programId, "boneWeight0");
    attributeBoneWeights[1] = glGetAttribLocation(programId, "boneWeight1");
    attributeBoneWeights[2] = glGetAttribLocation(programId, "boneWeight2");
    attributeBoneWeights[3] = glGetAttribLocation(programId, "boneWeight3");
    attributeTextures[0]    = glGetAttribLocation(programId, "vTex0");
    attributeTextures[1]    = glGetAttribLocation(programId, "vTex1");
    
    uniformTextures[Material::kTextureTypeDiffuse]  = glGetUniformLocation(programId, "diffuse");
    uniformTextures[Material::kTextureTypeNormal]   = glGetUniformLocation(programId, "normal");
    uniformTextures[Material::kTextureTypeSpecular] = glGetUniformLocation(programId, "specular");
    uniformTextures[Material::kTextureTypeBump]     = glGetUniformLocation(programId, "bump");
    
    for(int i = 0; i < 20; i++)
    {
        std::ostringstream b;
        b << "bones[" << i << "]" << std::ends;
        uniformBones[i] = glGetUniformLocation(programId, b.str().c_str());
    }
    
    uniformSkinned = glGetUniformLocation(programId, "skinned");
    
    /*cout << "---- Material Program Init Log ----" << endl;
    cout << "Attribute Vertex: " << AttributeVertex() << endl;
    cout << "Attribute Normal: " << AttributeNormal() << endl;
    cout << "Attribute TexCd0: " << attributeTextures[0] << endl;
    cout << "Attribute TexCd1: " << attributeTextures[1] << endl;
    cout << "Attribute BoneW0: " << attributeBoneWeights[0] << endl;
    cout << "Attribute BoneW1: " << attributeBoneWeights[1] << endl;
    cout << "Attribute BoneW2: " << attributeBoneWeights[2] << endl;
    cout << "Attribute BoneW3: " << attributeBoneWeights[3] << endl;
    for(int i = 0; i < 20; i++)
    {
        cout << "Uniform Bone[" << i << "]: " << uniformBones[i] << endl;
    }
    cout << "-----------------------------------" << endl;*/
}

const GLint MaterialProgram::AttributeBoneWeight(int idx)
{
    return attributeBoneWeights[idx];
}

const GLint MaterialProgram::AttributeTexture(int idx)
{
    return attributeTextures[idx];
}

const GLint MaterialProgram::UniformBones(int idx)
{
    return uniformBones[idx];
}

const GLint MaterialProgram::UniformTexture(Material::TextureType idx)
{
    return uniformTextures[idx];
}

const GLint MaterialProgram::UniformSkinned()
{
    return uniformSkinned;
}
