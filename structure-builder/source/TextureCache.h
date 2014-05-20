#pragma once

#include "GLTexture.h"
#include "stdafx.h"

class TextureCache {
	GLTexture * errorTexture;

	typedef pair<GLTexture*,double> cached;

	//The time of this current frame so that OS::Now() is not called every time a texture is retrieved
	//Represents OS::Now()+300 which is the amount of time before a texture is automatically uncached
	double expirationTime;
	unsigned int frameCount; //Only check if its time to clear out textures every 1024 frames
	map<string,cached> cachedTextures;
public:
	TextureCache();
	~TextureCache();

	//Acquires the given texture, caching it on the fly if necessary
	GLTexture * GetTexture(string path);

	//Caches the texture forever without unloading it until Flush() is explicitly called
	void PermaCacheTexture(string path);

	//Flush all textures out of cache including those perm-cached
	void Flush();

	//Automatically clean out textures which haven't been used for a long time
	//Must be called at the start of every frame to work properly
	void Refresh();
};