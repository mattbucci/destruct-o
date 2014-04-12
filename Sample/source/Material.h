/*
 *  Copyright 2014 Nathaniel Lewis, Matthew Bucci, Anthony Popelar, Brian Bamsch
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
    
    // Iterator type
    typedef std::map<TextureType, std::string>::const_iterator const_texture_iterator;
    
private:
    // Name of the material
    std::string id;
    
    // The textures this material uses
    std::map<TextureType, std::string> textures;
    
public:
    // Standard constructor - create an empty material
    Material();
    
    // Deserialization constructor - load from a Json serialized blob
    Material(const Json::Value& value, const std::string directory = ".");
    
    // Get the name of the material
    std::string& Id();
    
    // Get a const iterator to textures
    const_texture_iterator texturesBegin() const;
    const_texture_iterator texturesEnd() const;
    const_texture_iterator texturesFind(const TextureType& key) const;
};

#endif
