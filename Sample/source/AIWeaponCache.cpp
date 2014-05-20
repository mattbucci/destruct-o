#include "stdafx.h"
#include "AIWeaponCache.h"

AIWeaponCache::AIWeaponCache() : Autocacher("weapons/","weaponmanifest.json") {

}

WeaponData * AIWeaponCache::cacheItem(string path) {
	//Just use the save system to deserialize the class
	vector<unsigned char> saveData;
	lodepng::load_file(saveData,path);

	//Return a basic actorAI data if nothing loaded
	if (saveData.size() <= 0)
		return new WeaponData();

	//Load the data
	WeaponData * data = dynamic_cast<WeaponData*>(Savable::Deserialize(saveData,false));
	if (data == NULL)
		return new WeaponData();

	return data;
}

//Cleanup via delete
void AIWeaponCache::cleanup(WeaponData * valueToCleanup) {
	delete valueToCleanup;
}