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
#include "GLSkydome.h"

// Allocate a skybox with a particular name
GLSkydome::GLSkydome(std::string skyboxName, TextureCache& textureCache)
: vertexArrayObject(0), vertexBuffer(0), indexBuffer(0), skyboxName(skyboxName), previousProgram(NULL), textureCache(textureCache)
{

}

// Deconstructor releases all the OpenGL objects
GLSkydome::~GLSkydome()
{
    glDeleteVertexArrays(1, &vertexArrayObject);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
}

// Build the skybox
void GLSkydome::Build(GLushort divisions)
{
    // Generate the vertex data
    std::vector<vec3> vertices;
    std::vector<GLushort> indices;

    // Calculate the divisions for the sphere
    GLfloat dAzimuth = (GLfloat) M_PI / (GLfloat) (divisions);
    GLfloat dInclination = (GLfloat) M_PI / (GLfloat) (divisions);

    // Reserve space for the sphere's vertices
    vertices.reserve(2 + (divisions * 2)*(divisions - 1));

    // Push the top of the sphere vertex
    vertices.push_back(vec3(0,0,1));

    // Iterate through all the divisions of the inclination
    for(int i = 1; i < divisions; i++)
    {
        // Calculate the inclination
        GLfloat inclination = dInclination * (GLfloat) i;

        // Iterate through all divisions of the azimuth
        for(int j = 0; j < (divisions * 2); j++)
        {
            // Calculate the next longitude
            GLfloat azimuth = dAzimuth * (GLfloat) j;

            // We're generating the vertex of this point
            vec3 v;

            // Calculate the vertex
            v.x = sin(inclination) * cos(azimuth);
            v.y = sin(inclination) * sin(azimuth);
            v.z = cos(inclination);

            // Push back the first triangle
            vertices.push_back(v);
        }
    }

    // Push the bottom of the sphere vertex
    vertices.push_back(vec3(0,0,-1));

    // Generate the indices for the top of the sphere (special case)
    for(GLushort i = 0; i < (divisions * 2); i++ )
    {
        indices.push_back(i + 1);
        indices.push_back(0);

        // Handle wrapping
        if(i + 1 == (divisions * 2))
            indices.push_back(1);
        else
            indices.push_back(i + 2);
    }

    // Generate the indices for each rung of the sphere
    for(GLushort j = 0; j < divisions - 2; j++)
    {
        // Calculate the offset to this row
        GLushort offset = 1 + (j * (2 * divisions));

        // Calculate the offset to the next row
        GLushort noffset = 1 + ((j+1) * (2 * divisions));

        // Generate the indices
        for(GLushort i = 0; i < (divisions * 2); i++)
        {
            // Calculate the indices of each quad
            GLushort v0 = noffset + i;
            GLushort v1 = offset + i;
            GLushort v2 = 0, v3 = 0;

            // Do we need to wrap
            if(i + 1 == (divisions * 2))
            {
                // Loop to row beginning indices
                v2 = noffset;
                v3 = offset;
            } else
            {
                // Normal indices
                v2 = noffset + i + 1;
                v3 = offset + i + 1;
            }

            // Queue these triangles
            indices.push_back(v0);
            indices.push_back(v1);
            indices.push_back(v2);
            indices.push_back(v1);
            indices.push_back(v2);
            indices.push_back(v3);
        }
    }

    // Generate the indices for the bottom of the sphere (special case)
    GLushort offset = 1 + ((divisions-2) * (2 * divisions));
    for(int i = 0; i < (divisions * 2); i++)
    {
        indices.push_back(offset + i);
        indices.push_back(vertices.size() - 1);

        // Handle wrapping
        if(i + 1 == (divisions * 2))
            indices.push_back(offset);
        else
            indices.push_back(offset + i + 1);
    }

    // Generate the storage for the vertex data of the skybox
    glGenVertexArrays(1, &vertexArrayObject);

    // Upload the vertex data
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * vertices.size(), (GLvoid *) vertices.data(), GL_STATIC_DRAW);

    // Upload the index data
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indices.size(), (GLvoid *) indices.data(), GL_STATIC_DRAW);
    indexCount = indices.size();

    // Load the skybox texture
    textureCache.GetTexture<GLTextureCubeMap>(skyboxName);
}

// Draw the skybox
void GLSkydome::Draw(GL3DProgram * program, float distance)
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
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0);
}
