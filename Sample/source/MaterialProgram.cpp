/**
 * Copyright (c) 2016, Nathaniel R. Lewis, Anthony Popelar, Matt Bucci, Brian Bamsch
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

    uniformTextured = glGetUniformLocation(programId, "textured");
    uniformColorAmbient = glGetUniformLocation(programId, "material_color_ambient");
    uniformColorDiffuse = glGetUniformLocation(programId, "material_color_diffuse");

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

const GLint MaterialProgram::AttributeBoneWeight(int idx) const
{
    return attributeBoneWeights[idx];
}

const GLint MaterialProgram::AttributeTexture(int idx) const
{
    return attributeTextures[idx];
}

const GLint MaterialProgram::UniformBones(int idx) const
{
    return uniformBones[idx];
}

const GLint MaterialProgram::UniformTexture(Material::TextureType idx) const
{
    return uniformTextures[idx];
}

const GLint MaterialProgram::UniformSkinned() const
{
    return uniformSkinned;
}

// Textured
const GLint MaterialProgram::UniformTextured() const
{
    return uniformTextured;
}

// Colors
const GLint MaterialProgram::UniformColorAmbient() const
{
    return uniformColorAmbient;
}

const GLint MaterialProgram::UniformColorDiffuse() const
{
    return uniformColorDiffuse;
}
