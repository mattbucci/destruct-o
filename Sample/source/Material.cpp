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
#include "Material.h"

//#define __MATERIAL_PRINT_LOGS__

// Unique texture type count
const static unsigned int TextureTypeCount = 4;

// Since deserialization is a feature, we need to have the texture keys
const static std::string TextureTypeKeys[] =
{
    "DIFFUSE",
    "SPECULAR",
    "BUMP",
    "NORMAL",
};

// Standard constructor - create an empty material
Material::Material()
    : id(""), textures(Material::texture_store()), colorDiffuse(glm::vec3(1.0, 1.0, 1.0)), colorAmbient(glm::vec3(1.0, 1.0, 1.0)), opaque(true)
{

}

// Deserialization constructor - load from a Json serialized blob
Material::Material(const Json::Value& value, const std::string directory)
    : id(""), textures(Material::texture_store()), opaque(true)
{
    // We first need to validate that this a Json object
    if(!value.isObject())
    {
        throw std::runtime_error("Material::Material(const Json::Value& value) - value must be a object");
    }

    // Load the name of the material
    id = value["id"].asString();

    // Get the potential textures
    const Json::Value& bitmaps = value["textures"];

    // If this mesh contains a vertices entry (there could be some empty cases, for instance, an empty mesh was created, then serialized)
    if(!bitmaps.isNull() && bitmaps.isArray())
    {
        // Read in all the textures
        for(Json::Value::iterator tIt = bitmaps.begin(); tIt != bitmaps.end(); tIt++)
        {
            // Get a pointer to the texture type
            const std::string *it = std::find(&TextureTypeKeys[0], (&TextureTypeKeys[0]) + TextureTypeCount, (*tIt)["type"].asString());

            // Figure out the id
            TextureType type = (TextureType) std::distance(&TextureTypeKeys[0], it);

            // Add the texture
            textures[type] = directory + "/" + (*tIt)["filename"].asString();
        }
    }

    // Search for the diffuse color type
    const Json::Value& diffuse_color = value.get("diffuse", Json::Value::null);

    // Do we have a diffuse color
    if(!diffuse_color.isNull() && diffuse_color.isArray())
    {
        // Create a color storage vector and cache it
        colorDiffuse = glm::vec3(diffuse_color[0].asFloat(), diffuse_color[1].asFloat(), diffuse_color[2].asFloat());
    } else
    {
        colorDiffuse = glm::vec3(1.0, 1.0, 1.0);
    }

    // Search for particular color types
    const Json::Value& ambient_color = value.get("ambient", Json::Value::null);

    // Do we have a diffuse color
    if(!ambient_color.isNull() && ambient_color.isArray())
    {
        // Create a color storage vector and cache it
        colorAmbient = glm::vec3(ambient_color[0].asFloat(), ambient_color[1].asFloat(), ambient_color[2].asFloat());
    } else
    {
        colorAmbient = colorDiffuse;
    }

    // Search for opaque/transparent settings
    Json::Value opaque_default = true;
    opaque = value.get("opaque", opaque_default).asBool();

#if (defined __MATERIAL_PRINT_LOGS__)
    cout << "Loaded Material (" << id << "): ";
    cout << "Ambient => {" << colorAmbient.r << " " << colorAmbient.g << " " << colorAmbient.b << "} ";
    cout << "Diffuse => {"  << colorDiffuse.r << " " << colorDiffuse.g << " " << colorDiffuse.b << "} ";
    cout << "Textures (" << textures.size() << ")" << endl;
#endif
}

// Get the name of the material
std::string& Material::Id()
{
    return id;
}

// Get a const reference to the texture storage
const Material::texture_store& Material::Textures() const
{
    return textures;
}

// Get the material colors
const vec3& Material::ColorDiffuse() const
{
    return colorDiffuse;
}

const vec3& Material::ColorAmbient() const
{
    return colorAmbient;
}

const bool& Material::Opaque() const
{
    return opaque;
}
