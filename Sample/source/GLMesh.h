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

#include "stdafx.h"
#include "GL3DProgram.h"

// Class to encapsulate draw data loaded from model files
class GLMesh
{
protected:
    // Contains information about an individual mesh slice
    struct Submesh
    {
        // Vertex array object for this submesh
        GLuint attributes;
        
        // Vertex attribute buffers for the mesh
        union
        {
            // The buffers we require for data storage
            GLuint buffers[4];
            
            // The names of the buffers
            struct
            {
                GLuint vertexBuffer;
                GLuint normalBuffer;
                GLuint textureCoordinateBuffer;
                GLuint indexBuffer;
            };
        };
        
        // Material id for this mesh
        GLuint material;
        
        // Constructor to setup everything about this mesh
        Submesh();
        ~Submesh();
    };
    
    // Submeshes supported by this
    std::vector<Submesh *> submeshes;
    
    // Last program used to render (used to figure out if we've cached the vertex locations)
    GL3DProgram *priorShader;
    
public:
    // Load a model from a file
    GLMesh(const std::string& filename);
    
    // Draw this mesh
    void Draw(GL3DProgram *shader);
};

#endif
