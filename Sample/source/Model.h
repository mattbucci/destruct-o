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

#ifndef __MODEL_H__
#define __MODEL_H__

#include "stdafx.h"

#include "VertexAttributes.h"
#include "Mesh.h"
#include "Transform.h"
#include "Node.h"
#include "Material.h"
#include "Animation.h"

#include "TextureCache.h"
#include "MaterialProgram.h"

// I chose to make this a define because the optimiser will perform
// loop unwinding on any for loops made using it.  
#define MaxBoneWeights 4

// Loads and renders models
class Model
{
public:
    // Iterator types
    typedef std::map<std::string, Animation *>::iterator animation_iterator;
    typedef std::map<std::string, Animation *>::const_iterator animation_const_iterator;
    
private:
    // Data structure to represent a bone
    struct Bone
    {
        // Name of the bone
        std::string id;
        
        // Transform of the bone
        Transform   transform;
        
        // Inverse of the bone transform
        glm::mat4   inverseTransformMatrix;
        
        // Standard constructors
        Bone();
        Bone(const Json::Value& value);
    };
    
    // Class which handles rendering mesh components
    struct MeshPartRenderData
    {
        // Mesh part we are rendering
        Mesh::Part          *meshpart;
        
        // Material for this mesh part
        Material            *material;
        
        // Bones associated with this mesh part
        std::vector<Bone *>  bones;
        
        // Vertex array object for this mesh part
        GLuint               attributes;
        
        // Vertex buffer object for the index data
        GLuint               indices;
        
        // Standard constructor
        MeshPartRenderData();
        ~MeshPartRenderData();
    };
    
    // Animations of this model
    std::map<std::string, Animation *>   animations;
    
    // Materials of this model
    std::map<std::string, Material *>    materials;
    
    // Meshes belonging to this model
    std::vector<Mesh *>                  meshes;
    
    // Root node of the model
    Node                                *skeleton;

	// flattened node list of the model
	std::map<std::string,Node *>		 nodes;
    
    // The mesh part renderers
    std::vector<MeshPartRenderData *>    renderables;
    
    // The texture cache used by the parent object
    TextureCache&                        textureCache;
    
    // Map of meshes to their vertex buffer objects
    std::map<Mesh *, GLuint>             buffers;
    
    // Previous shader used to render (do we need to update vao)
    MaterialProgram                     *previousProgram;
    bool                                 uploaded;
    
    // Helper function to load animations
    void loadAnimations(const Json::Value& root);
    
    // Helper function to load materials
    void loadMaterials(const Json::Value& root, const std::string& directory);
    
    // Helper function to load meshes
    void loadMeshes(const Json::Value& root);
    
    // Helper function to load nodes
    void loadSkeleton(const Json::Value& root);
    
    // Helper function to load parts (technically, not the right way, but fuck it.  We'll just assume the mesh
    // is in the right spot, because 99.99999% of the time it is).  The correct way is to map all the internal
    // nodes into a global transform tree.  This just assumes all parts being rendered on on static nodes
    // within the space of the model file
    void loadParts(const Json::Value& root);
    void loadPartsNodeSearch(const Json::Value& node);
    
public:
    // Create an empty model for population manually
    Model(TextureCache& _textureCache);
    
    // Create a model by deserializing it from a json file
    Model(const Json::Value& root, const std::string& directory, TextureCache& _textureCache);
    
    // Standard deconstructor - free all heap allocated data
    ~Model();
    
    // Upload the data to the GPU
    void Upload();
    
    // Update the model (animations and such)
    void Update(double delta, double now);
    
    // Render the model
    void Draw(MaterialProgram *program);
    void Draw(MaterialProgram *program, const Node& _skeleton);
    
    // Accessor method for the skeleton
    const Node* Skeleton() const;

	// Accessor method for fast node lookup list
	Node * FastNodeLookup(const string & node);
    
    // Accessor method for the animations
    const std::map<std::string, Animation *>& Animations() const;
    
    // Construct a model object from a model file
    static Model* LoadFromJsonFile(const std::string& directory, const std::string& name, TextureCache &_textureCache);
    static Model* LoadFromCompressedJsonFile(const std::string& directory, const std::string& name, TextureCache &_textureCache);
};

#endif
