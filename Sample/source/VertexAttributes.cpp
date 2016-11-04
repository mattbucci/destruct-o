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
    : vertexAttributesFrameSize(0)
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

    // Check against all possible vertex attributes (if there is a better way of doing this please tell me)
    unsigned int i = 0;
    for(i = 0; i < VertexAttributeCount; i++)
    {
        // If these keys match
        if(key.find(VertexAttributeStorageKeys[i]) == 0)
        {
			// Storage class
			Attribute attribute;
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

// Get the size of an attribute
const size_t& VertexAttributes::AttributeSize(AttributeKey key)
{
    return VertexAttributeStorageKeySize[key];
}

// Get iterators
VertexAttributes::iterator VertexAttributes::begin()
{
    return attributes.begin();
}

VertexAttributes::iterator VertexAttributes::end()
{
    return attributes.end();
}

VertexAttributes::const_iterator VertexAttributes::begin() const
{
    return attributes.begin();
}

VertexAttributes::const_iterator VertexAttributes::end() const
{
    return attributes.end();
}
