#include "stdafx.h"
#include "TextureCache.h"
#include "OS.h"

TextureCache::TextureCache()
{
	frameCount = 0;
}

TextureCache::~TextureCache()
{
	//Uncache all cached textures
	for (auto it = cachedTextures.begin(); it != cachedTextures.end(); )
    {
		// Uncache only if its not the error texture (erased separately)
		delete it->second.first;
		it = cachedTextures.erase(it);
	}
}

//Automatically clean out textures which haven't been used for a long time
//Must be called every frame to work properly
void TextureCache::Refresh()
{
	double frameTime = OS::Now();
	
	frameCount++; //Overflows safely
	if (frameCount % 1024 == 0) {
		for (auto it = cachedTextures.begin(); it != cachedTextures.end(); ) {
			//Textures without expiration have an expiration of < 0 so check
			//for that first
			if ((it->second.second > 0) && (it->second.second > frameTime)) {
				//Uncache
				delete it->second.first;
				it = cachedTextures.erase(it);
			}
			else
				it++;
		}
	}

	expirationTime = frameTime+300;
}

//Flush all textures out of cache including those perm-cached
void TextureCache::Flush()
{
	for (auto it = cachedTextures.begin(); it != cachedTextures.end(); )
    {
		//Uncache
		delete it->second.first;
		it = cachedTextures.erase(it);
	}
}