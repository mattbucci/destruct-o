#pragma once

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