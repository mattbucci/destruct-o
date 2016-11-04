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
