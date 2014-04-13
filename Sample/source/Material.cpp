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

#include "Material.h"

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
    : id("")
{
    
}

// Deserialization constructor - load from a Json serialized blob
Material::Material(const Json::Value& value, const std::string directory)
    : Material::Material()
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
    if(bitmaps != Json::Value::null && bitmaps.isArray())
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
}

// Get the name of the material
std::string& Material::Id()
{
    return id;
}

// Get a const iterator to textures
Material::const_texture_iterator Material::texturesBegin() const
{
    return textures.begin();
}

Material::const_texture_iterator Material::texturesEnd() const
{
    return textures.end();
}

Material::const_texture_iterator Material::texturesFind(const TextureType& key) const
{
    return textures.find(key);
}
