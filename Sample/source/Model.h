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
        // Node this bone is attached to
        std::string          id;

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

    // The mesh part renderers
    std::vector<MeshPartRenderData *>    renderables;

    // The texture cache used by the parent object
    TextureCache&                        textureCache;

    // Map of meshes to their vertex buffer objects
    std::map<Mesh *, GLuint>             buffers;

    // Previous shader used to render (do we need to update vao)
    MaterialProgram                     *previousProgram;

    // Flag indicating if the model data has been uploaded to the gpu
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

    // Accessor method for the animations
    const std::map<std::string, Animation *>& Animations() const;

    // Construct a model object from a model file
    static Model* LoadFromJsonFile(const std::string& directory, const std::string& name, const Transform& offset, TextureCache &_textureCache);
    static Model* LoadFromCompressedJsonFile(const std::string& directory, const std::string& name, const Transform& offset, TextureCache &_textureCache);
};

#endif
