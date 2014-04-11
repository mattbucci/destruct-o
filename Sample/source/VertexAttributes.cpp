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

#include "VertexAttributes.h"

// Unique vertex attribute count
const static unsigned int VertexAttributeCount = 5;

// Since deserialization is a feature, we need to have the mesh keys available
const static std::string VertexAttributeStorageKeys[] =
{
    "POSITION",
    "NORMAL",
    "COLOR",
    "TEXCOORD",
    "BLENDWEIGHT",
};

// Data size of the elements of the interleaven attribute data
const static size_t VertexAttributeStorageKeySize[] =
{
    3,
    3,
    4,
    2,
    2,
};

// Standard constructor (created an empty object, in order to build one from scratch)
VertexAttributes::VertexAttributes ()
    : vertexAttributesFrameSize(0)
{
    
}

// Deserialization constructor
VertexAttributes::VertexAttributes(const Json::Value& value)
    : VertexAttributes::VertexAttributes()
{
    // We first need to validate that this a Json array
    if(!value.isArray())
    {
        throw std::runtime_error("VertexAttributes::VertexAttributes(const Json::Value& value) - value must be an array");
    }
    
    // Iterate through all the Json values
    for(Json::Value::iterator it = value.begin(); it != value.end(); it++)
    {
        // The vertex attribute must be a string
        if(!(*it).isString())
        {
            throw std::runtime_error("VertexAttributes::VertexAttributes(const Json::Value& value) - serialized vertex attributes must be strings");
        }
        
        // Get the vertex attribute
        std::string vertexAttribute = (*it).asString();
        
        // Add this attribute by its key
        addVertexAttributeByStorageKey(vertexAttribute);
    }
}

// Add a vertex attribute by key
void VertexAttributes::addVertexAttributeByStorageKey(const std::string& key)
{
    // Storage class
    Attribute attribute;
    
    // Check against all possible vertex attributes (if there is a better way of doing this please tell me)
    unsigned int i = 0;
    for(i = 0; i < VertexAttributeCount; i++)
    {
        // If these keys match
        if(key.find(VertexAttributeStorageKeys[i]) == 0)
        {
            // If this key is numeric
            if(key.length() > VertexAttributeStorageKeys[i].length())
            {
                // Get the numeric section (end of attribute name + 1 -> key end
                const std::string numeric = key.substr(VertexAttributeStorageKeys[i].length(), key.length() - VertexAttributeStorageKeys[i].length());
                
                // Convert into an integer
                attribute.index = std::atoi(numeric.c_str());
            }
            
            // Store the attribute type
            attribute.attribute = static_cast<AttributeKey>(i);
            
            // Store the offset
            attribute.offset = vertexAttributesFrameSize;
            
            // Increase the frame size by the size of this attribute
            vertexAttributesFrameSize += VertexAttributeStorageKeySize[i];
            
            // Add the attribute to the list of them
            attributes.push_back(attribute);
            
            // We're done, see yall later
            break;
        }
    }
    
    // Check for failure
    if(i == VertexAttributeCount)
    {
        throw std::runtime_error("void VertexAttributes::addVertexAttributeByStorageKey(std::string key) - vertex attribute unrecognized");
    }
}

// Get the currently added vertex attributes
const std::vector<VertexAttributes::Attribute>& VertexAttributes::Attributes()
{
    return attributes;
}

// Get the size of the attribute frame
const size_t& VertexAttributes::AttributeFrameSize()
{
    return vertexAttributesFrameSize;
}
