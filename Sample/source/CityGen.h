#ifndef CITYGEN_H
#define CITYGEN_H

#include "GameTile.h"
#include "Building.h"

//the size of each city
extern const int citysize;

class SavableCityData;

class CityGen
{
	vector<function<SavableCityData*(GameTile* tile, vec3 pos)>> generationFunctions;
	//Place a turret perch at the given location
	static void placeTurretPerch(TileCell * cells, float turretHeight, vec3 tileOffset, vec2 turretPos, SavableCityData * city);
	//Place a spire in the center of the city
	static void placeCenterSpire(TileCell * cells, float groundHeight, vec3 tileOffset, vec3 cityPos, SavableCityData * city);
	//Check if a point is within a city centered at 0,0
	template <class T>
	static bool pointInCity(T point) {
		if ((point.x < -citysize/2.0) || (point.y < -citysize/2.0))
			return false;
		if ((point.x > citysize/2.0) || (point.y > citysize/2.0))
			return false;
		return true;
	}

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
