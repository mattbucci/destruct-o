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

#ifndef __MESH_H__
#define __MESH_H__

#include "stdafx.h"
#include "VertexAttributes.h"

// The mesh is an object which
class Mesh
{
public:
    // Subclass which handles subcomponents of the mesh (they are just index buffers though)
    struct Part
    {
        // Owning mesh (for buffer data lookup)
        Mesh *mesh;

        // Mesh parts are assigned names
        std::string id;

        // Mesh parts only consist of an index buffer
        std::vector<GLushort> indices;

        // Base initialization just nullifies everything
        Part();

        // Deserialize a mesh part from Json
        Part(const Json::Value& value);
    };

private:
    // For performance reasons (at least according to the iOS profiler), mesh data is interleaved
    std::vector<GLfloat> data;

    // Indices into the interleaven vertex data
    std::vector<GLushort>  indices;

    // Offset calculation for interleaven data
    VertexAttributes     attributes;

    // The individual components of the mesh (separated by material, vertex count, etc.)
    std::vector<Part *>  parts;

public:
    // Create an empty mesh for population manually
    Mesh();

    // Create a mesh by deserializing it from Json
    Mesh(const Json::Value& value);

    // Standard deconstructor - frees all heap allocated data
    ~Mesh();

    // Get references to the mesh data
    std::vector<GLfloat>& Data();
    std::vector<GLushort>&  Indices();
    VertexAttributes&     Attributes();
    std::vector<Part *>&  Parts();
};

#endif
