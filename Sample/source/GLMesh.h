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

#ifndef __GL_MESH_H__
#define __GL_MESH_H__

#define MaxBoneWeights 4

#include "stdafx.h"
#include "GLMeshProgram.h"
#include "TextureCache.h"

// Class to encapsulate draw data loaded from model files
class GLMesh
{
protected:
    // Contains information about an individual mesh slice
    struct Part
    {
        // Vertex array object for this submesh
        GLuint       attributes;
        
        // Vertex buffer object for this part's index data
        GLuint       indexBuffer;
        
        // Index count
        unsigned int indexCount;
        
        // Constructor to setup everything about this mesh
        Part();
        ~Part();
    };
    
    // Contains information about a render step of the mesh
    struct RenderStep
    {
        // Vector of bone objects
        std::vector<std::pair<std::string, mat4> > bones;
        
        // The mesh component name for this render step
        std::string meshpartid;
        
        // The material name for this render step
        std::string materialid;
    };
    
    // Vertex buffer object containing all mesh verticies
    GLuint vertexBuffer;
    
    // Attribute offsets in vertex buffer (since its all interleaven)
    GLint  vertexFrameSize;
    GLint  positionOffset;
    GLint  normalOffset;
    GLint  texCoordOffset;
    GLint  boneWeightOffset[MaxBoneWeights];
    
    // Constituant parts of this mesh
    std::map<std::string, Part *> parts;
    
    // Materials that are supported by the mesh
    std::map<std::string, std::vector<std::string> > materials;
    
    // The render steps of the mesh (list of part name + material name)
    std::vector<RenderStep *> rendersteps;
    
    // Active texture cache
    TextureCache&             textureCache;
    
    // Default scale of mesh
    vec3                      scale;
    
    // Last program used to render (used to figure out if we've cached the vertex locations)
    GLMeshProgram              *priorShader;
    
public:
    // Load a model from a file
    GLMesh(const std::string& directory, const std::string& name, TextureCache& _textureCache, vec3 _scale = vec3(1,1,1));
    ~GLMesh();
    
    // Draw this mesh
    void Draw(GLMeshProgram *shader);
};

#endif
