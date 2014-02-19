#include "stdafx.h"
#include "TextureCache.h"
#include "OS.h"

TextureCache::TextureCache() {
	errorTexture = GLTexture::GenerateErrorTexture();
	frameCount = 0;
}
TextureCache::~TextureCache() {
	//Uncache all cached textures
	for (auto it = cachedTextures.begin(); it != cachedTextures.end(); ) {
		//Uncache only if its not the error texture (erased separately)
		if (it->second.first != errorTexture)
			delete it->second.first;
		it = cachedTextures.erase(it);
	}
	//Destroy the error texture
	delete errorTexture;
}

void TextureCache::setSearchPath(string path) {
	rootSearchPath = path;
}

//Acquires the given texture, caching it on the fly if necessary
GLTexture * TextureCache::GetTexture(string path) {
	//Check cache first
	auto it = cachedTextures.find(path);
	if (it == cachedTextures.end()) {
		//Attempt to cache
		GLTexture * tex = new GLTexture(rootSearchPath+"\\"+path);
		if (!tex->CacheTexture()) {
			//Cache failed, destroy the texture
			//Substitute the memory-created error texture
			//cache the error texture to avoid redundant errors
			delete tex;
			//Errors are perma-cached
			//this is expected behavior since GetTexture() could be called after a permacache request
			cachedTextures[path] = cached(errorTexture,-1);
			return errorTexture;
		}
		//Add in cache with expiration date
		cachedTextures[path] = cached(tex,expirationTime);
		return tex;
	}
	else {
		//Only update the time stamp if its not set to perma cache
		if (it->second.second > 0)
			it->second.second = expirationTime;
		return it->second.first;
	}
		
}

//Automatically clean out textures which haven't been used for a long time
//Must be called every frame to work properly
void TextureCache::Refresh() {
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

void TextureCache::PermaCacheTexture(string path) {
	//Attempt to cache
	GLTexture * tex = new GLTexture(rootSearchPath+"\\"+path);
	if (!tex->CacheTexture()) {
		//Cache failed, destroy the texture
		delete tex;
		return;
	}
	//Add in cache without expiration
	cachedTextures[path] = cached(tex,-1);
}

//Flush all textures out of cache including those perm-cached
void TextureCache::Flush() {
	for (auto it = cachedTextures.begin(); it != cachedTextures.end(); ) {
		//Uncache
		delete it->second.first;
		it = cachedTextures.erase(it);
	}
}