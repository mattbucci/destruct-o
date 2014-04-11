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
}
