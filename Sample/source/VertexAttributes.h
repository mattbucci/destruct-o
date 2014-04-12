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

#ifndef __VERTEX_ATTRIBUTES_H__
#define __VERTEX_ATTRIBUTES_H__

#include "stdafx.h"
#include <json/json.h>

// The G3DJ mesh format defines a multitude of vertex attributes that will
// be fed into shaders.  The vertex attributes describe the order the
// associated data appears in the interleaven vertex attribute data array
// stored by a mesh.  Vertex attributes will cal

class VertexAttributes
{
public:
    // Types of vertex attributes
    typedef enum : unsigned char
    {
        kAttributeVertex            = 0,
        kAttributeNormal            = 1,
        kAttributeColor             = 2,
        kAttributeTextureCoordinate = 3,
        kAttributeBoneWeight        = 4,
    } AttributeKey;
    
    // Vertex Attribute type
    struct Attribute
    {
        // The key for the attribute
        AttributeKey attribute;
        
        // The numeric identifier for the key (tex coord 0, bone weight 5, etc.)
        unsigned int index;
        
        // The offset into the vertex attribute data of this attribute
        size_t       offset;
        
        // Empty constructor
        Attribute()
            : attribute(kAttributeVertex), index(0), offset(0)
        {
            
        }
        
        // Simple constructor to set all data at once
        Attribute(AttributeKey _attribute, unsigned int _index, size_t _offset)
            : attribute(_attribute), index(_index), offset(_offset)
        {
            
        }
    };
    
    // Iterator types
    typedef std::vector<Attribute>::iterator       iterator;
    typedef std::vector<Attribute>::const_iterator const_iterator;
    
private:
    // Current vertex data frame size (to keep track of offsets)
    size_t vertexAttributesFrameSize;
    
    // List of the vertex attributes added to this object
    std::vector<Attribute> attributes;
    
public:
    // Create a blank vertex attributes object
    VertexAttributes();
    
    // Load vertex attributes from a Json serialized object
    VertexAttributes(const Json::Value& value);
    
    // Add a vertex attribute
    void addVertexAttributeByStorageKey(const std::string& key);
    
    // Read only reference to the frame size of the attributes
    const size_t& AttributeFrameSize();
    
    // Read only reference to the size of an attribute type
    static const size_t& AttributeSize(AttributeKey key);
    
    // Read only reference to the attributes
    const std::vector<Attribute>& Attributes();
    
    // Get iterators
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
};

#endif
