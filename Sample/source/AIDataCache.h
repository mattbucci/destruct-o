
#pragma once

#include "ActorAIData.h"
#include "Autocacher.h"

class AIDataCache : public Autocacher<ActorAIData*> {
protected:
	//Cache actor ai data
	ActorAIData * cacheItem(string path) override;
	//Cleanup via delete
	void cleanup(ActorAIData * valueToCleanup);
public:
	AIDataCache();
};