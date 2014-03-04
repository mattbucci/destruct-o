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

#include "GLMesh.h"

// Open asset importer library
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/vector3.h>

// Default constructor (loads all submeshes from model file)
GLMesh::GLMesh(const std::string& filename)
{
    // Create an importer object for asset importer
    Assimp::Importer Importer;
    
    // Load a model with the asset importer
    const aiScene* pScene = Importer.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
    
    // If the import failed, we have a problem
    if(!pScene)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error parsing '%s': '%s'\n", filename.c_str(), Importer.GetErrorString());
        throw std::exception();
    }
    
    // Allocate the necessary amount of submesh pointers
    submeshes.resize(pScene->mNumMeshes);
    
    // Initialize all of the submeshes
    for(unsigned int i = 0; i < pScene->mNumMeshes; i++)
    {
        // Get a pointer to the mesh in question
        const aiMesh *mesh = pScene->mMeshes[i];
        
        // Create a submesh object
        Submesh *submesh = new Submesh();
        
        // Set the material id
        submesh->material = mesh->mMaterialIndex;
        
        // Copy the vertex data from the mesh
        glBindBuffer(GL_ARRAY_BUFFER, submesh->vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(aiVector3D), mesh->mVertices, GL_STATIC_DRAW);
        
        // Copy the normal data form the mesh
        glBindBuffer(GL_ARRAY_BUFFER, submesh->normalBuffer);
        glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(aiVector3D), mesh->mNormals, GL_STATIC_DRAW);
        
        // Copy the texture coordinates from the mesh
        glBindBuffer(GL_ARRAY_BUFFER, submesh->textureCoordinateBuffer);
        glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(aiVector3D), mesh->mTextureCoords[0], GL_STATIC_DRAW);
        
        // Build the index buffer
        std::vector<unsigned int> indexBuffer;
        for(unsigned j = 0; j < mesh->mNumFaces; j++)
        {
            // Get the face
            const aiFace       *face = &(mesh->mFaces[j]);
            const unsigned int *it   = face->mIndices;
            
            // Make sure this face has the correct number of indices
            _ASSERTE(face->mNumIndices == 3);
            
            // Insert this data into the index buffer
            indexBuffer.insert(indexBuffer.end(), it, it+3);
        }
        
        // Upload the index buffer to the GPU
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, submesh->indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexBuffer.size(), indexBuffer.data(), GL_STATIC_DRAW);
        
        // Add this submesh to the mesh
        submeshes.push_back(submesh);
    }
    
    // Load textures for this model
    for(unsigned int i = 0; i < pScene->mNumMaterials; i++)
    {
        
    }
    
    // Success
    SDL_Log("Loaded mesh: \"%s\"\n", filename.c_str());
}

// Initialization of the submesh object
GLMesh::Submesh::Submesh()
{
#if !(defined __ANDROID__)
    // Generate a vertex array
    glGenVertexArrays(1, &attributes);
#endif
    
    // Generate the buffers for the submesh
    glGenBuffers(4, buffers);
}

// Deletion of the submesh object
GLMesh::Submesh::~Submesh()
{
#if !(defined __ANDROID__)
    // Generate a vertex array
    glDeleteVertexArrays(1, &attributes);
#endif
    // Delete the buffers
    glDeleteBuffers(4, buffers);
}