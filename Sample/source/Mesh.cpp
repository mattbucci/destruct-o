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
