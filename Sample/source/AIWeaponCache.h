
#pragma once

#include "WeaponData.h"
#include "Autocacher.h"

class AIWeaponCache : public Autocacher<WeaponData*> {
protected:
	//Cache actor ai data
	WeaponData * cacheItem(string path) override;
	//Cleanup via delete
	void cleanup(WeaponData * valueToCleanup);
public:
	AIWeaponCache();
};