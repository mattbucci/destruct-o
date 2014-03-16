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
GLMesh::GLMesh(const std::string& directory, const std::string& name, TextureCache& _textureCache) : textureCache(_textureCache)
{
    
    // Get the filename
    std::string filename = directory + "/" + name;
    
    // Create an importer object for asset importer
    Assimp::Importer Importer;
    
    // Load a model with the asset importer
    const aiScene* pScene = Importer.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
    
    // If the import failed, we have a problem
    if(!pScene)
    {
        std::cerr << "Error parsing " << filename << ": " << Importer.GetErrorString() << std::endl;
        throw std::exception();
    }
    
    // Initialize all of the submeshes
    std::cout << "Loading mesh: " << filename << std::endl;
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
        submesh->indexCount = indexBuffer.size();
        
        // Add this submesh to the mesh
        submeshes.push_back(submesh);
        std::cout << "  Loaded submesh@" << i + 1 << " with " << mesh->mNumFaces << " faces" << std::endl;
    }
    
    // Load textures for this model
    for (unsigned int i = 0 ; i < pScene->mNumMaterials ; i++)
    {
        // Get a pointer to the material
        const aiMaterial* material = pScene->mMaterials[i];
        
        // Path of the texture to use
        std::string path = "error.texture.this.should.fail";
        
        //std::cout << "Ambient = " << material->GetTextureCount(aiTextureType_AMBIENT) << std::endl;
        //std::cout << "Diffuse = " << material->GetTextureCount(aiTextureType_DIFFUSE) << std::endl;
        //std::cout << "Normals = " << material->GetTextureCount(aiTextureType_NORMALS) << std::endl;
        //std::cout << "Emissive = " << material->GetTextureCount(aiTextureType_EMISSIVE) << std::endl;
        
        // If this maerial has textures associated with it
        if (material->GetTextureCount(aiTextureType_DIFFUSE))
        {
            // Get the name of the texture
            aiString Path;
            
            // Get the texture name
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
            {
                // Calculate the path to the mesh
                path = directory + "/" + Path.data;
                std::cout << "    Using texture: " << path << std::endl;
                
                // Cache this texture
                textureCache.GetTexture(path);
            }
        }

        // Push back this path
        textures.push_back(path);
        
        // Yay material
        std::cout << "  Loaded material@" << i + 1 << std::endl;
    }
    
    
    std::cout << "Aniamtion count = " << pScene->mNumAnimations << std::endl;
    aiAnimation *animation = pScene->mAnimations[0];
    std::cout << animation->mName.C_Str() << ", Ticks = " << animation->mDuration << ", per second = " << animation->mTicksPerSecond << std::endl;
    
    
    // Success
    std::cout << "Loaded mesh: \"" << filename.c_str() << "\"" << std::endl;
}

// Destroy this mesh
GLMesh::~GLMesh()
{
    // Iterate through all the submeshes
    for(std::vector<Submesh *>::iterator it = submeshes.begin(); it != submeshes.end(); ++it)
    {
        delete *it;
    }
}

// Draw this mesh
void GLMesh::Draw(GL3DProgram *shader)
{
    // Iterate through all the submeshes
    for(std::vector<Submesh *>::iterator it = submeshes.begin(); it != submeshes.end(); ++it)
    {
#if !(defined __ANDROID__)
        // Bind the vertex array object
        glBindVertexArray((*it)->attributes);

        // If the shader has changed, reload the vertex array object pointers
        if(shader != priorShader)
        {
            // Enable the vertex array object attributes that we will be using
            glEnableVertexAttribArray(shader->AttributeVertex());
            glEnableVertexAttribArray(shader->AttributeNormal());
            glEnableVertexAttribArray(shader->AttributeTexture());
#endif
            // Set the vertex attribute pointer
            glBindBuffer(GL_ARRAY_BUFFER, (*it)->vertexBuffer);
            glVertexAttribPointer(shader->AttributeVertex(), 3, GL_FLOAT, GL_FALSE, 0, 0);
            
            // Set the normal attribute pointer
            glBindBuffer(GL_ARRAY_BUFFER, (*it)->normalBuffer);
            glVertexAttribPointer(shader->AttributeNormal(), 3, GL_FLOAT, GL_FALSE, 0, 0);
            
            // Set the texture coordinate attribute pointer
            glBindBuffer(GL_ARRAY_BUFFER, (*it)->textureCoordinateBuffer);
            glVertexAttribPointer(shader->AttributeTexture(), 3, GL_FLOAT, GL_FALSE, 0, 0);
            
            // Set the index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*it)->indexBuffer);
            
#if !(defined __ANDROID__)
        }
#endif
        // Bind the texture
        glActiveTexture(GL_TEXTURE0);
        textureCache.GetTexture(*(textures.begin() + (*it)->material))->Bind();
        
        // Set the reflectivity
        glm::vec2 specular = glm::vec2(1.0, 1.0);
        glUniform2fv(glGetUniformLocation(shader->GetId(), "material_reflectivity"), 2, (const GLfloat *) &specular);
        
        // Draw this mesh
        glDrawElements(GL_TRIANGLES, (*it)->indexCount, GL_UNSIGNED_INT, 0);
    }
    
    // Store the new shader pointer
    priorShader = shader;
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