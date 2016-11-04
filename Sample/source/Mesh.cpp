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
#include "Mesh.h"

// Empty mesh constructor, doesn't do much of anything
Mesh::Mesh()
    : data(0, 0), indices(0, 0), attributes(VertexAttributes())
{

}

// Deserialize a Json encoded mesh
Mesh::Mesh(const Json::Value& value)
    : data(0, 0), indices(0, 0), attributes(VertexAttributes())
{
    // We first need to validate that this a Json object
    if(!value.isObject())
    {
        throw std::runtime_error("Mesh::Mesh(const Json::Value& value) - value must be a object");
    }

    // Get the potential vertex attributes
    const Json::Value& attrs = value["attributes"];

    // If this mesh contains a vertex attributes entry (empty if no data i suppose)
    if(attrs != Json::Value::null)
    {
        // Initialize the attributes
        attributes = VertexAttributes(attrs);
    }

    // Get the potential vertices
    const Json::Value& vertices = value["vertices"];

    // If this mesh contains a vertices entry (there could be some empty cases, for instance, an empty mesh was created, then serialized)
    if(vertices != Json::Value::null && vertices.isArray())
    {
        // Reserve the needed space for the vertices
		data.reserve(vertices.size());

        // Pull in all the vertices from the Json blob
        for(Json::Value::iterator vIt = vertices.begin(); vIt != vertices.end(); vIt++)
        {
            data.push_back((*vIt).asFloat());
        }
    }

    // Get the potential parts of the mesh
    const Json::Value& meshparts = value["parts"];

    // If this mesh contains a parts entry (there could be some empty cases, for instance, a small static mesh or an uninialized mesh)
    if(meshparts != Json::Value::null && meshparts.isArray())
    {
        // Iterate through all of the parts attached to this mesh
        for(Json::Value::iterator pIt = meshparts.begin(); pIt != meshparts.end(); pIt++)
        {
            // Allocate a mesh part, deserialize the Json blob
            Mesh::Part *part = new Mesh::Part(*pIt);

            // Add the indices from this part into the global index buffer
            indices.reserve(indices.size() + part->indices.size());
            indices.insert(indices.end(), part->indices.begin(), part->indices.end());

            // Push this part onto the parts list
            part->mesh = this;
            parts.push_back(part);
        }
    }
}

// Deallocate all heap allocated memory
Mesh::~Mesh()
{
    // Iterate through all mesh parts and free the data
    for(std::vector<Mesh::Part *>::iterator it = parts.begin(); it != parts.end(); it++)
    {
        delete *it;
    }
}

// Standard empty part constructor.  Exists to make things initialized
Mesh::Part::Part()
    : id(""), indices(0, 0)
{

}

// Deserialization constructor
Mesh::Part::Part(const Json::Value& value)
    : indices(0, 0)
{
    // We first need to validate that this a Json object
    if(!value.isObject())
    {
        throw std::runtime_error("Mesh::Part::Part(const Json::Value& value) - value must be a object");
    }

    // The data type MUST be triangles
    if(!(value["type"].asString() == "TRIANGLES"))
    {
        throw std::runtime_error("Mesh::Part::Part(const Json::Value& value) - mesh data must be in triangle format");
    }

    // Retrieve the stored mesh part name
    id = value["id"].asString();

    // Retrieve the indices
    const Json::Value& partindices = value["indices"];

    // We must have an indices entry
    if(partindices == Json::Value::null)
    {
        throw std::runtime_error("Mesh::Part::Part(const Json::Value& value) - mesh data must contain indices");
    }

    // Iterate through all the indices, push them to the index array
    for(Json::Value::iterator iIt = partindices.begin(); iIt != partindices.end(); iIt++)
    {
        indices.push_back((*iIt).asUInt());
    }
}

// Get references to the mesh data
std::vector<GLfloat>& Mesh::Data()
{
    return data;
}

std::vector<GLushort>& Mesh::Indices()
{
    return indices;
}

VertexAttributes& Mesh::Attributes()
{
    return attributes;
}

std::vector<Mesh::Part *>& Mesh::Parts()
{
    return parts;
}
