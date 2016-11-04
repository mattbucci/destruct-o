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
