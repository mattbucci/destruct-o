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

#ifndef __VERTEX_ATTRIBUTES_H__
#define __VERTEX_ATTRIBUTES_H__

#include "stdafx.h"
#include <json/json.h>

/**
 * The G3DJ mesh format defines a multitude of vertex attributes that will
 * be fed into shaders.  The vertex attributes describe the order the
 * associated data appears in the interleaven vertex attribute data array
 * stored by a mesh.
 */

class VertexAttributes
{
public:
    /**
     * Types of vertex shader input attributes
     */
    typedef enum : unsigned char
    {
        kAttributeVertex            = 0,    /**< Position input attribute */
        kAttributeNormal            = 1,    /**< Normal input attribute */
        kAttributeColor             = 2,    /**< Color input attribute */
        kAttributeTextureCoordinate = 3,    /**< Texture coordinate input attribute */
        kAttributeBoneWeight        = 4,    /**< Bone and blendweight input attribute */
    } AttributeKey;
    
    /**
     * Vertex attribute management class
     */
    struct Attribute
    {
        /** Input attribute type */
        AttributeKey attribute;
        
        /** Input attribute index (texture coordinate 1, boneweight 3, etc.)*/
        unsigned int index;
        
        /** Offset into the vertex attribute frame */
        size_t       offset;
        
        /**
         * Standard Constructor.  Initializes all data members to null
         */
        Attribute()
            : attribute(kAttributeVertex), index(0), offset(0)
        {
            
        }
        
        /**
         * Constructor to initialize all data members to provided parameters
         * @param _attribute attribute type
         * @param _index attribute index
         * @param _offset offset into the vertex attribute frame for this attribute
         */
        Attribute(AttributeKey _attribute, unsigned int _index, size_t _offset)
            : attribute(_attribute), index(_index), offset(_offset)
        {
            
        }
    };
    
    // Iterator types
    typedef std::vector<Attribute>::iterator       iterator;
    typedef std::vector<Attribute>::const_iterator const_iterator;
    
private:
    /** Current vertex data frame size (to keep track of offsets) */
    size_t vertexAttributesFrameSize;
    
    /** List of the vertex attributes added to this object */
    std::vector<Attribute> attributes;
    
public:
    /** Create a blank vertex attributes object */
    VertexAttributes();
    
    /** 
     * Load vertex attributes from a Json serialized object
     * @param value Json array to deserialize vertex attributes table from
     */
    VertexAttributes(const Json::Value& value);
    
    /** Add a vertex attribute */
    void addVertexAttributeByStorageKey(const std::string& key);
    
    /** Read only reference to the frame size of the attributes */
    const size_t& AttributeFrameSize();
    
    /** Read only reference to the size of an attribute type */
    static const size_t& AttributeSize(AttributeKey key);
    
    /** Read only reference to the attributes */
    const std::vector<Attribute>& Attributes();
    
    // Get iterators
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
};

#endif
