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
#include "GLTextureCubeMap.h"
#include "lodepng.h"

// The string contants representing the suffixes of the cubemap textures
static const std::string cubemapTextureNameSuffixes[] =
{
    "+x.png", "-x.png", "+y.png", "-y.png", "+z.png", "-z.png"
};

// The string contants representing the suffixes of the cubemap textures
static const GLenum cubemapTextureTypes[] =
{
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
};

// The count of important things to load
static const size_t cubemapTextureCount = 6;

// Load a cubemap (appends identifiers e.g. +z.png, -x.png, etc.)
GLTextureCubeMap::GLTextureCubeMap(string texturePath)
    : GLTexture(texturePath)
{
    
}

// Create an error cubemap
GLTextureCubeMap* GLTextureCubeMap::GenerateErrorTexture()
{
    return NULL;
}

// Attempt to cache the texture in graphics memory
bool GLTextureCubeMap::CacheTexture()
{
    // If the texture has already been cached, get out of here
	if (IsCached())
    {
		return true;
    }
    
    // Generate an cubemap OpenGL Texture
	glEnable(GL_TEXTURE_CUBE_MAP);
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
#if !(defined __MOBILE__)
    // If we are not on mobile we need to specify wrapper the r texture coordinate
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
#endif
    
    // Loop through and load all the cubemap textures
    for(int i = 0; i < cubemapTextureCount; i++)
    {
        // Load the texture from disk
        std::vector<unsigned char> textureData;
        textureData.clear();
        std::string textureName = texturePathName + cubemapTextureNameSuffixes[i];
        unsigned error = lodepng::decode(textureData, width, height, textureName);
        if(error)
        {
            // Error and cleanup
            cout << "Cubemap Texture: " << textureName << " Failed to load" << endl;
            glDeleteTextures(1, &textureId);
            return false;
        }
        
        // Upload the texture to the GPU
        glTexImage2D(cubemapTextureTypes[i], 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *) textureData.data());
    }
    
    // Return whether this operation was successful
    return (textureId > 0);
}

// Bind this texture to the currently selected texture unit as a cube map
void GLTextureCubeMap::Bind()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
}
