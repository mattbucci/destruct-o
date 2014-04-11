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
    // Load materials
    loadMaterials(root);
    
    // Load mesh data
    loadMeshes(root);
    
    // Load node data
    loadNodes(root);
    
    // Load parts
    loadParts(root);
    
    // Success
    std::cout << "Loaded model: \"" << filename.c_str() << "\"" << std::endl;
}

Model::~Model()
{
    // Iterate through all meshes and free the data
    for(std::vector<Mesh *>::iterator it = meshes.begin(); it != meshes.end(); it++)
    {
        delete *it;
    }
}

// Helper function to load the materials from the serialized Json blob
void Model::loadMaterials(const Json::Value &root)
{
    // Get the value for the mesh entry
    const Json::Value& mats = root["materials"];
    
    // If the model file contains mesh entries
    if(mats != Json::Value::null)
    {
        // Iterate through all the mesh entries
        for(Json::Value::iterator it = mats.begin(); it != mats.end(); it++)
        {
            // Load a material from the entry
            Material *material = new Material(*it);
            
            // Store this material for later usage
            materials[material->Id()] = material;
        }
    }
}

// Helper function to load the mesh data from the serialized Json blob
void Model::loadMeshes(const Json::Value& root)
{
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
}

// Helper function to load the node tree from the serialized json data
void Model::loadNodes(const Json::Value &root)
{
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
}

// Helper function to load the parts from the serialized json data
void Model::loadParts(const Json::Value &root)
{
    // Get potential node entries
    const Json::Value& nodes = root["nodes"];
    
    // If the model contains node entries
    if(nodes != Json::Value::null)
    {
        // Loop through all the nodes and check them for parts
        for(Json::Value::iterator it = nodes.begin(); it != nodes.end(); it++)
        {
            loadPartsNodeSearch(*it);
        }
    }
}

// Recursive worker for the parts loader
void Model::loadPartsNodeSearch(const Json::Value &node)
{
    // Break out if this node is invalid
    if(node == Json::Value::null || !node.isObject())
    {
        return;
    }
    
    // Get the id for global inverse lookup
    std::string id = node["id"].asString();
    
    // If we find that we have a parts entry, explore it
    const Json::Value& parts = node["parts"];
    if(parts != Json::Value::null)
    {
        for(Json::Value::iterator it = parts.begin(); it != parts.end(); it++)
        {
            // Allocate a part to store data in
            Model::MeshPartRenderData *renderable = new Model::MeshPartRenderData();
            
            // Look up the mesh part this renderable uses
            std::string meshpartid = (*it)["meshpartid"].asString();
            for(std::vector<Mesh *>::iterator mIt = meshes.begin(); mIt != meshes.end(); mIt++)
            {
                // Iterate through mesh parts and put them in a map
                for(std::vector<Mesh::Part *>::iterator pIt = (*mIt)->Parts().begin(); pIt != (*mIt)->Parts().end(); pIt++)
                {
                    // If this is the part we are looking for
                    if((*pIt)->id == meshpartid)
                    {
                        // Store this mesh part
                        renderable->meshpart = *pIt;
                        
                        // get the fuck out (nested loop, can't break, need to force the iterators to the end)
                        mIt = meshes.end() - 1;
                        break;
                    }
                }
            }
            
            // Lookup the material this mesh uses
            std::string materialid = (*it)["materialid"].asString();
            renderable->material = materials[materialid];
            
            // Load the bones
            const Json::Value& bones = (*it)["bones"];
            if(bones != Json::Value::null)
            {
                for(Json::Value::iterator bIt = bones.begin(); bIt != bones.end(); bIt++)
                {
                    // Allocate a bone and deserialize
                    Model::Bone *bone = new Model::Bone(*bIt);
                    
                    // Store in the bones list
                    renderable->bones.push_back(bone);
                }
            }
            
            // Store this renderable data in our renderable list
            renderables.push_back(renderable);
        }
    }
    
    // Explore children if we have any
    const Json::Value& childs = node["children"];
    if(childs != Json::Value::null)
    {
        for(Json::Value::iterator it = childs.begin(); it != childs.end(); it++)
        {
            // Explore the children
            loadPartsNodeSearch(*it);
        }
    }
}

// Standard constructor (initialize everything)
Model::Bone::Bone()
    : id(""), transform(Transform())
{
    
}

// Deserialization constructor
Model::Bone::Bone(const Json::Value& value)
    : Model::Bone::Bone()
{
    // We first need to validate that this a Json object
    if(!value.isObject())
    {
        throw std::runtime_error("Model::Bone::Bone(const Json::Value& value) - value must be a object");
    }
    
    // Load the name of the material
    id = value["node"].asString();
    
    // Load the transform
    transform = Transform(value);
}

// Standard constructor
Model::MeshPartRenderData::MeshPartRenderData()
    : meshpart(NULL), material(NULL), bones(0, NULL), attributes(0), indices(0)
{
    
}

Model::MeshPartRenderData::~MeshPartRenderData()
{
    // Iterate through mesh parts and put them in a map
    for(std::vector<Model::Bone *>::iterator bIt = bones.begin(); bIt != bones.end(); bIt++)
    {
        delete *bIt;
    }
}
