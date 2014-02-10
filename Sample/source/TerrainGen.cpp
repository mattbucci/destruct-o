#include <noise/noise.h>
#include "noiseutils.h"
#include "TerrainGen.h"

TerrainGen::TerrainGen() {
	hBuilder.SetSourceModule(noiseMod);
	hBuilder.SetDestNoiseMap(hMap);
	tilesx = 256;
	tilesy = 256;
}
TerrainGen::~TerrainGen() {
}

void TerrainGen::setSeed(int seed) {
	this->seed = seed;
	noiseMod.SetSeed(seed);
}

void TerrainGen::setTileScale(double x, double y) {
	tilesx = x;
	tilesy = y;
}

void TerrainGen::setTileSize(int x, int y) {
	tilex = x;
	tiley = y;
	hBuilder.SetDestSize(x, y);
}

unsigned char* TerrainGen::generateTile(int x, int y) {
	unsigned char* d = (unsigned char*)malloc(sizeof(char)*tilex*tiley);
	hBuilder.SetBounds(x * tilesx, x * tilesx + tilesx, y * tilesy, y * tilesy + tilesy);
	hBuilder.Build();
	
	//memcpy(a, hMap.GetSlabPtr(), tilex * tiley);
	//For now, quick and painful conversion from float to int.
	float* s = hMap.GetSlabPtr();
	for(int i = 0; i < tilex * tiley; i++) {
		d[i] = (unsigned char)(s[i] * 127 + 128);
	}

	return d;
}
