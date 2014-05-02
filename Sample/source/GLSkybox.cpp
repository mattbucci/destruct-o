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
#include "GLSkybox.h"

// The vertices in a cube
static const GLfloat cube_vertices[] =
{
    -1.0,  1.0,  1.0,
    -1.0, -1.0,  1.0,
    1.0, -1.0,  1.0,
    1.0,  1.0,  1.0,
    -1.0,  1.0, -1.0,
    -1.0, -1.0, -1.0,
    1.0, -1.0, -1.0,
    1.0,  1.0, -1.0,
};

// The indices in a cube
static const GLushort cube_indices[] =
{
    0, 1, 2,
    2, 3, 0,
    3, 2, 6,
    6, 7, 3,
    7, 6, 5,
    5, 4, 7,
    4, 5, 1,
    1, 0, 4,
    0, 3, 7,
    7, 4, 0,
    1, 2, 6,
    6, 5, 1
};

// Allocate a skybox with a particular name
GLSkybox::GLSkybox(std::string skyboxName, TextureCache& textureCache)
    : vertexArrayObject(0), vertexBuffer(0), indexBuffer(0), skyboxName(skyboxName), previousProgram(NULL), textureCache(textureCache)
{
    
}

// Deconstructor releases all the OpenGL objects
GLSkybox::~GLSkybox()
{
    glDeleteVertexArrays(1, &vertexArrayObject);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
}

// Build the skybox
void GLSkybox::Build()
{
    // Generate the storage for the vertex data of the skybox
    glGenVertexArrays(1, &vertexArrayObject);
    
    // Upload the vertex data
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    
    // Upload the index data
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    
    // Load the skybox texture
    textureCache.GetTexture<GLTextureCubeMap>(skyboxName);
}

// Draw the skybox
void GLSkybox::Draw(GL3DProgram * program)
{
    // Bind the vertex array object
    glBindVertexArray(vertexArrayObject);
    
    // If the program has changed
    if(program != previousProgram)
    {
        // Bind the vertices to the VAO
        glEnableVertexAttribArray(program->AttributeVertex());
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(program->AttributeVertex(), 3, GL_FLOAT, GL_FALSE, 0, 0);
        
        // Bind the indices to the VAO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    }
    
    // Center the skybox
    program->Model.Reset();
    program->Model.Apply();
    
    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    textureCache.GetTexture<GLTextureCubeMap>(skyboxName)->Bind();
    
    // Draw the skybox (at the most distant distance)
    glDepthRangef(1, 1);
    glDrawElements(GL_TRIANGLES, sizeof(cube_indices) / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
    glDepthRangef(0, 1);
}
