#include "stdafx.h"
#include "AIDataCache.h"

AIDataCache::AIDataCache() : Autocacher("ai/","aimanifest.json") {

}

ActorAIData * AIDataCache::cacheItem(string path) {
	//Just use the save system to deserialize the class
	vector<unsigned char> saveData;
	lodepng::load_file(saveData,path);

	//Return a basic actorAI data if nothing loaded
	if (saveData.size() <= 0)
		return new ActorAIData();

	//Load the data
	ActorAIData * data = dynamic_cast<ActorAIData*>(Savable::Deserialize(saveData,false));
	if (data == NULL)
		return new ActorAIData();

	return data;
}

//Cleanup via delete
void AIDataCache::cleanup(ActorAIData * valueToCleanup) {
	delete valueToCleanup;
}