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

#ifndef __GL_TEXTURE_H__
#define __GL_TEXTURE_H__

#include "stdafx.h"

class GLTexture
{
protected:
	GLuint textureId;

	string texturePathName;

	unsigned int width,height;

	friend class GLMaterialManager;
	friend class GL2DProgram;
public:
	GLTexture(string texturePathName);
	virtual ~GLTexture();

	//Flags for desired settings
	enum textureFlags {
		//Uses GL_LINEAR
		TEXTURE_LINEAR = 0,
		//uses GL_NEAREST for everything
		TEXTURE_FASTEST = 1,
		//Uses mipmap if possible, this is the default
		TEXTURE_MIPMAP = 2,
	};

	//Create an error texture which relies only on memory
	//do not attempt to cache
	static GLTexture * GenerateErrorTexture();

	//Attempt to cache the texture in graphics memory
	virtual bool CacheTexture(textureFlags flags = TEXTURE_MIPMAP);

	//Default texture options
	static const textureFlags DefaultTextureOption;

	//Whether or not this texture is cached currently
	bool IsCached();

	//Allow the user to get properties describing this texture
	float GetWidth();
	float GetHeight();

	// Bind this texture to the currently selected texture unit
	virtual void Bind();
};

#endif
