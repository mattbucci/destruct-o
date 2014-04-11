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

#include "Model.h"
#include <fstream>

// Create an empty model for population manually
Model::Model()
    : meshes(0, NULL), node(new Node())
{
    
}

// Create a model by deserializing it from Json
Model::Model(const std::string& directory, const std::string& name)
    : Model::Model()
{
    // Open the Json file for reading
    std::string filename = directory + "/" + name;
    std::ifstream document;
    document.open(filename);
    
    // Create a json parser for this file
    Json::Reader reader;
    Json::Value  root;
    
    // Read the model file
    if(!reader.parse(document, root))
    {
        std::cerr << "Error parsing " << filename << std::endl;
        throw std::exception();
    }
    
    // Get the value for the mesh entry
    const Json::Value& meshes = root["meshes"];
    
    // If the model file contains mesh entries
    if(meshes != Json::Value::null)
    {
        // Iterate through all the mesh entries
        for(Json::Value::iterator it = meshes.begin(); it != meshes.end(); it++)
        {
            // Load a mesh from the json blob
            Mesh *mesh = new Mesh(*it);
            
            // Hold on to the mesh
            this->meshes.push_back(mesh);
        }
    }
    
    // Get potential node entries
    const Json::Value& nodes = root["nodes"];
    
    // If the model contains node entries
    if(nodes != Json::Value::null)
    {
        // Create the node heirchy before search for mesh rendering information
        for(Json::Value::iterator it = nodes.begin(); it != nodes.end(); it++)
        {
            // Create a new child node
            Node *child = new Node(*it, node);
            
            // Add this child to our list
            node->AddChild(child, false);
        }
    }
    
    // Success
    std::cout << "Loaded model: \"" << filename.c_str() << "\"" << std::endl;
}
