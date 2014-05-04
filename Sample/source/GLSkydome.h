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

#ifndef __GL_SKYDOME__
#define __GL_SKYDOME__

#include "stdafx.h"
#include "GLTextureCubeMap.h"
#include "GL3DProgram.h"
#include "TextureCache.h"

class GLSkydome
{
    // The GL state variables
    GLuint vertexArrayObject;
    GLuint vertexBuffer;
    GLuint indexBuffer;
    GLuint vertexCount;
    
    // The name of the skybox texture
    std::string skyboxName;
    
    // Previous program used to render
    GL3DProgram *previousProgram;
    
    // Texture cache
    TextureCache& textureCache;
    
public:
    // Allocate a skybox with a particular name
    GLSkydome(std::string skyboxName, TextureCache& textureCache);
    ~GLSkydome();
    
    // Build the skybox
    void Build(unsigned int divisions);
    
    // Draw the skydome
    void Draw(GL3DProgram * program, float distance);
};

#endif
