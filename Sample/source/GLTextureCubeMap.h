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

#ifndef __GL_TEXTURE_CUBEMAP_H__
#define __GL_TEXTURE_CUBEMAP_H__

#include "stdafx.h"
#include "GLTexture.h"

class GLTextureCubeMap : public GLTexture
{
public:
    // Load a cubemap (appends identifiers e.g. +z.png, -x.png, etc.)
	GLTextureCubeMap(string texturePath);
    
	// Create an error cubemap
	static GLTextureCubeMap * GenerateErrorTexture();
    
	// Attempt to cache the texture in graphics memory
	bool CacheTexture() override;
    
	// Bind this texture to the currently selected texture unit
	void Bind() override;
};

#endif
