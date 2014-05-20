#pragma once

#include "GLTexture.h"
#include "stdafx.h"

class TextureCache
{
	typedef pair<GLTexture*,double> cached;

	//The time of this current frame so that OS::Now() is not called every time a texture is retrieved
	//Represents OS::Now()+300 which is the amount of time before a texture is automatically uncached
	double expirationTime;
	unsigned int frameCount; //Only check if its time to clear out textures every 1024 frames
	map<string,cached> cachedTextures;
public:
	TextureCache();
	~TextureCache();
    
    // Acquires the given texture, caching it on the fly if necessary
    template <class T>
    T * GetTexture(string path, typename T::textureFlags cacheFlags = T::DefaultTextureOption)
    {
        //Check cache first
        auto it = cachedTextures.find(path);
        if (it == cachedTextures.end())
        {
            //Attempt to cache
            GLTexture * tex = new T(path);
            if (!tex->CacheTexture(cacheFlags))
            {
                //Cache failed, destroy the texture
                //Substitute the memory-created error texture
                //cache the error texture to avoid redundant errors
                delete tex;
                
                //Errors are perma-cached
                //this is expected behavior since GetTexture() could be called after a permacache request
                T * errorTexture = T::GenerateErrorTexture();
                cachedTextures[path] = cached(errorTexture, -1);
                return errorTexture;
            }
            
            //Add in cache with expiration date
            cachedTextures[path] = cached(tex,expirationTime);
            return dynamic_cast<T*> (tex);
        }
        else
        {
            //Only update the time stamp if its not set to perma cache
            if (it->second.second > 0)
                it->second.second = expirationTime;
            return dynamic_cast<T*> (it->second.first);
        }
    }

	// Caches the texture forever without unloading it until Flush() is explicitly called
    template <typename T>
    void PermaCacheTexture(string path, typename T::textureFlags cacheFlags = T::DefaultTextureOption)
    {
        //Attempt to cache
        GLTexture * tex = new T(path);
        if (!tex->CacheTexture(cacheFlags)) {
            //Cache failed, destroy the texture
            delete tex;
            return;
        }
        //Add in cache without expiration
        cachedTextures[path] = cached(tex,-1);
    }

	//Flush all textures out of cache including those perm-cached
	void Flush();

	//Automatically clean out textures which haven't been used for a long time
	//Must be called at the start of every frame to work properly
	void Refresh();
};