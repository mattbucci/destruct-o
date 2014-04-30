
#pragma once

#include "stdafx.h"
#include "lodepng.h"

template <class T>
class Autocacher {
	string cacheRoot;
	string manifestName;

	//map filename to data
	map<string,T> cache;
protected:
	//If cleanup is necessary this should be overriden with the correct cleanup function
	virtual void cleanup(T valueToCleanup) {

	}
	//cache one item given the path to that item
	virtual T cacheItem(string path) = 0;
public:
	Autocacher(string cacheRoot,string manifestName) {
		this->cacheRoot = cacheRoot;
		this->manifestName = manifestName;
	}
	virtual ~Autocacher() {
		for (auto m : cache)
			cleanup(m.second);
	}

	//Retrieve one item which has been cached
	T& GetCached(string filename) {
		_ASSERTE(cache.find(filename) != cache.end());
		return cache[filename];
	}

	//Load everything in the manifest into the cache
	void Load() {
		cout << "Caching manifest: " << cacheRoot << manifestName << "\n";
		vector<unsigned char> fileData;
		lodepng::load_file(fileData,cacheRoot + manifestName);
		//Parse json
		Json::Value root;
		Json::Reader reader;
		reader.parse(string((char*)fileData.data(),fileData.size()),root,false);
		//Load all the particle files listed
		for (auto file : root) {
			string fileName = file.asString();
			cout << "Caching: " << fileName << "\n";
			cache[fileName] = cacheItem(cacheRoot + fileName);
		}
	}
	
};