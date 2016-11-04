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
    program->Apply();

    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    textureCache.GetTexture<GLTextureCubeMap>(skyboxName)->Bind();

    // Draw the skybox (at the most distant distance)
    glDepthRangef(1, 1);
    glDrawElements(GL_TRIANGLES, sizeof(cube_indices) / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
    glDepthRangef(0, 1);
}

// Draw the skybox
void GLSkybox::Draw(GL3DProgram * program, float distance)
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
    program->Model.Scale(distance, distance, distance);
    program->Apply();

    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    textureCache.GetTexture<GLTextureCubeMap>(skyboxName)->Bind();

    // Draw the skybox (at the most distant distance)
    glDrawElements(GL_TRIANGLES, sizeof(cube_indices) / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
}
