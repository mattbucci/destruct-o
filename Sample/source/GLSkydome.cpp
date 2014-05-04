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
void GLSkydome::Build(unsigned int divisions)
{
    // Calculate the divisions for the sphere
    GLfloat dAzimuth = (GLfloat) M_2_PI / (GLfloat) (divisions * 2);
    GLfloat dInclination = (GLfloat) M_PI / (GLfloat) (divisions);
    
    // Generate the vertex data
    std::vector<vec3> vertices;
    
    // Iterate through all the latitudes
    for(int i = 0; i < divisions; i++)
    {
        // Calculate the next latitude
        GLfloat inclination = dInclination * (GLfloat) i;
        GLfloat nInclination = inclination + dInclination;
        
        // Iterate through all the longitudes
        for(int j = 0; j < (divisions * 2); j++)
        {
            // Calculate the next longitude
            GLfloat azimuth = dAzimuth * (GLfloat) j;
            GLfloat nAzimuth = azimuth + dAzimuth;
            
            // Calculate the vertices
            vec3 v1, v2, v3, v4;
            
            // Calculate the base point
            v1.x = sin(inclination) * cos(azimuth);
            v1.y = sin(inclination) * sin(azimuth);
            v1.z = cos(inclination);
            
            // Calculate the next point along the inclination
            v2.x = sin(nInclination) * cos(azimuth);
            v2.y = sin(nInclination) * sin(azimuth);
            v2.z = cos(nInclination);
            
            // Calculate the next point along the azimuth & inclination
            v3.x = sin(nInclination) * cos(nAzimuth);
            v3.y = sin(nInclination) * sin(nAzimuth);
            v3.z = cos(nInclination);
            
            // Calculate the next point along the azimuth
            v4.x = sin(inclination) * cos(nAzimuth);
            v4.y = sin(inclination) * sin(nAzimuth);
            v4.z = cos(inclination);
            
            // Push back the first triangle
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
            vertices.push_back(v1);
            vertices.push_back(v3);
            vertices.push_back(v4);
        }
    }
    vertexCount = vertices.size();
    
    // Generate the storage for the vertex data of the skybox
    glGenVertexArrays(1, &vertexArrayObject);
    
    // Upload the vertex data
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * vertices.size(), (GLvoid *) vertices.data(), GL_STATIC_DRAW);
    
    // Upload the index data
    /*glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);*/
    
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
    //glDrawElements(GL_TRIANGLES, sizeof(cube_indices) / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}
