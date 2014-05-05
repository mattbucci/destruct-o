#ifndef CITYGEN_H
#define CITYGEN_H

#include "GameTile.h"
#include "Building.h"

class SavableCityData;

class CityGen {
	vector<function<SavableCityData*(GameTile* tile, vec3 pos)>> generationFunctions;
public:
	CityGen();

	void GenerateCities(GameTile* tile);
	void construct_city(GameTile * tile, vec3 pos);
	void construct_road(GameTile* tile, vec3 start, vec3 end);
	void construct_building(GameTile* tile, vec3 pos);
	void generatecitylocations(GameTile* tile);
	//Utility function for calculating index into buffer
	int BufferIndex(GameTile * tile,int x, int y);
};
#endif
