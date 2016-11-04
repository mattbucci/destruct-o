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

#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "stdafx.h"
#include <json/json.h>

// The mesh is an object which
class Material
{
public:
    // Types of material textures
    typedef enum : unsigned char
    {
        kTextureTypeDiffuse  = 0,
        kTextureTypeSpecular = 1,
        kTextureTypeBump     = 2,
        kTextureTypeNormal   = 3,
    } TextureType;

    // Types of color attributes
    typedef enum : unsigned char
    {
        kColorAttributeAmbient    = 0,
        kColorAttributeDiffuse    = 1,
        kColorAttributeSpecular   = 2,
        kColorAttributeEmissive   = 3,
        kColorAttributeReflection = 4,
    } ColorAttribute;

    // Texture storage type
    typedef std::map<TextureType, std::string> texture_store;

    // Iterator type
    typedef std::map<ColorAttribute, glm::vec4> color_store;

private:
    // Name of the material
    std::string id;

    // The textures this material uses
    texture_store textures;

    // Material colors
    vec3 colorDiffuse;
    vec3 colorAmbient;

    // Is this material opaque
    bool opaque;

public:
    // Standard constructor - create an empty material
    Material();

    // Deserialization constructor - load from a Json serialized blob
    Material(const Json::Value& value, const std::string directory = ".");

    // Get the name of the material
    std::string& Id();

    // Get a const reference to the texture storage
    const texture_store& Textures() const;

    // Get the material colors
    const vec3& ColorDiffuse() const;
    const vec3& ColorAmbient() const;

    // Get if the material is opaque
    const bool& Opaque() const;
};

#endif
