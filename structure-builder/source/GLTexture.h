#pragma once

#include "stdafx.h"

class GLTexture {
	GLuint textureId;

	string texturePathName;

	unsigned int width,height;

	friend class GLMaterialManager;
	friend class GL2DProgram;
public:
	GLTexture(string texturePathName);
	~GLTexture();

	//Create an error texture which relies only on memory
	//do not attempt to cache
	static GLTexture * GenerateErrorTexture();

	//Attempt to cache the texture in graphics memory
	bool CacheTexture();

	//Whether or not this texture is cached currently
	bool IsCached();

	//Allow the user to get properties describing this texture
	float GetWidth();
	float GetHeight();

	//Bind this texture to the 0 texture unit
	//if you need to bind to a different texture unit, add an override
	void Bind();
};