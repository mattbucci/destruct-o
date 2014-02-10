#ifndef TERRAINGEN_H
#define TERRAINGEN_H

#include <noise/noise.h>
#include "noiseutils.h"

class TerrainGen {
	int seed;
	double tilesx, tilesy;
	int tilex, tiley;
	noise::module::Perlin noiseMod;
	noise::utils::NoiseMap hMap;
	noise::utils::NoiseMapBuilderPlane hBuilder;

public:
	TerrainGen();
	~TerrainGen();

	void setSeed(int seed);
	void setTileScale(double x, double y);
	void setTileSize(int x, int y);
	unsigned char* generateTile(int x, int y);
};

#endif
