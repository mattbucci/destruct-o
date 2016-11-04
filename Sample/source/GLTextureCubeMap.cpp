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
#include "GLTextureCubeMap.h"
#include "lodepng.h"

//Default texture options
const GLTextureCubeMap::textureFlags GLTextureCubeMap::DefaultTextureOption = GLTextureCubeMap::TEXTURE_LINEAR;

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
bool GLTextureCubeMap::CacheTexture(textureFlags flags)
{
    // If the texture has already been cached, get out of here
	if (IsCached())
    {
		return true;
    }

    // Generate an cubemap OpenGL Texture
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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
