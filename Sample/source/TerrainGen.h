#ifndef TERRAINGEN_H
#define TERRAINGEN_H

#include <noise/noise.h>
#include "noiseutils.h"

class TerrainGen {
	int seed;
	double tilesx, tilesy;
	int tilex, tiley;

	// Flat Terrain Generator Modules
	noise::module::Billow baseFlatNoise;
	noise::module::ScaleBias flatNoise;

	// Hill Generator Modules
	noise::module::Perlin baseHillNoise;
	noise::module::ScaleBias hillNoise;

	// Blend Level 1 Modules
	noise::module::Perlin blend1Noise;
	noise::module::Select blend1Blender;

	// Mountain Generator Module
	noise::module::RidgedMulti baseMountNoise;
	noise::module::ScaleBias mountNoise;

	// Blend Level 2 Modules
	noise::module::Perlin blend2Noise;
	noise::module::Select blend2Blender;

	// Final Small Detail
	noise::module::Perlin baseDetailNoise;
	noise::module::ScaleBias detailNoise;

	// Blend Level 3 Modules
	noise::module::Add blend3Blender;

public:
	TerrainGen();
	~TerrainGen();

	int getSeed();
	void setSeed(int seed);
	void setTileScale(double x, double y);
	void setTileSize(int x, int y);
	unsigned char* generateTile(int x, int y);
};

#endif
