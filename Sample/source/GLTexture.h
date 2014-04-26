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

	//Create an error texture which relies only on memory
	//do not attempt to cache
	static GLTexture * GenerateErrorTexture();

	//Attempt to cache the texture in graphics memory
	virtual bool CacheTexture();

	//Whether or not this texture is cached currently
	bool IsCached();

	//Allow the user to get properties describing this texture
	float GetWidth();
	float GetHeight();

	// Bind this texture to the currently selected texture unit
	virtual void Bind();
};