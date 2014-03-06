#include "stdafx.h"
#include <noise/noise.h>
#include "noiseutils.h"
#include "TerrainGen.h"
#include <ctime>
#include <iostream>
#include <climits>


TerrainGen::TerrainGen() {

	//Set up Blender Module Params
	blend1Noise.SetFrequency(3.0);
	blend1Noise.SetPersistence(0.0);
	blend2Noise.SetOctaveCount(2);
	blend2Noise.SetFrequency(1.0);
	blend2Noise.SetPersistence(0.5);

	//Set up Flat Terrain Generation
	baseFlatNoise.SetFrequency(2.0);
	baseFlatNoise.SetOctaveCount(3);
	flatNoise.SetSourceModule(0, baseFlatNoise);
	flatNoise.SetScale(0.025);
	flatNoise.SetBias(-0.8);

	//Set up Hill Terrain Generation
	baseHillNoise.SetFrequency(0.75);
	baseHillNoise.SetOctaveCount(3);
	hillNoise.SetSourceModule(0, baseHillNoise);
	hillNoise.SetScale(0.50);
	hillNoise.SetBias(-0.55);

	//Set up Blend Level 1
	blend1Blender.SetSourceModule(0, flatNoise);
	blend1Blender.SetSourceModule(1, hillNoise);
	blend1Blender.SetControlModule(blend1Noise);
	blend1Blender.SetBounds(0.0, 1000.0);
	blend1Blender.SetEdgeFalloff(1.00);

	//Set up Mountain Terrain Generation
	mountNoise.SetSourceModule(0, baseMountNoise);
	mountNoise.SetBias(-0.5);

	//Set up Blend Level 2
	blend2Blender.SetSourceModule(0, blend1Blender);
	blend2Blender.SetSourceModule(1, mountNoise);
	blend2Blender.SetControlModule(blend2Noise);
	blend2Blender.SetBounds(0.5, 1000.0);
	blend2Blender.SetEdgeFalloff(0.5);

	//Set up Fine Detail Noise
	baseDetailNoise.SetOctaveCount(2);
	baseDetailNoise.SetFrequency(40.0);
	baseDetailNoise.SetPersistence(0.25);
	detailNoise.SetSourceModule(0, baseDetailNoise);
	detailNoise.SetScale(0.015);
	detailNoise.SetBias(0.0);

	blend3Blender.SetSourceModule(0, blend2Blender);
	blend3Blender.SetSourceModule(1, detailNoise);

	//Initialize Material Builder
	//matMapBuilder.SetSourceModule(matBlend);
	//matMapBuilder.SetDestNoiseMap(matMap);

	//Initialize Heightmap Builder
	heightMapBuilder.SetSourceModule(blend3Blender);
	heightMapBuilder.SetDestNoiseMap(heightMap);

	//Default Scale and Size Variables
	tilesx = 1;
	tilesy = 1;
	tilex = 256;
	tiley = 256;

	//Set Default Dest Size
	heightMapBuilder.SetDestSize(tilex, tiley);

	//Randomly Seed Terrain
	time_t t;
	time(&t);
	srand(t);
    setSeed(0);
}
TerrainGen::~TerrainGen() {
}

void TerrainGen::setSeed(int seed) {
	this->seed = seed;
	baseFlatNoise.SetSeed(seed);
	baseHillNoise.SetSeed(seed);
	baseMountNoise.SetSeed(seed);
	blend1Noise.SetSeed(seed);
	blend2Noise.SetSeed(seed/2);
}


int TerrainGen::getSeed() {
	return this->seed;
}

void TerrainGen::setTileScale(double x, double y) {
	tilesx = x;
	tilesy = y;
}

void TerrainGen::setTileSize(int x, int y) {
	tilex = x;
	tiley = y;
	heightMapBuilder.SetDestSize(tilex, tiley);
}

unsigned char* TerrainGen::generateTile(int x, int y) {
    _ASSERTE(getSeed() == 0);
	unsigned char* d = (unsigned char*)malloc(sizeof(char)*tilex*tiley);
	double bx0 = x * tilesx;
	double bx1 = bx0 + tilesx;
	double by0 = y * tilesx;
	double by1 = by0 + tilesy;

	//Setup and Build Tile to be Generated
	heightMapBuilder.SetBounds(bx0, bx1, by0, by1);
	heightMapBuilder.Build();

	//Get Pointers to Raw Terrain Data
	float* rawTerrain = heightMap.GetSlabPtr();
	//float* rawMaterial = matMap.GetSlabPtr();

	//Allocate Space for Compiled Tile
	int s = tilex * tiley;
	unsigned char* tile = (unsigned char*)malloc(sizeof(unsigned char) * s * 4);

	//Compile Tile Data
	for(int i = 0; i < s; i++) {
		tile[(i*4)] = (unsigned char)(rawTerrain[i] * 86 + 128);
		tile[(i*4) + 1] = 2; // TODO: Intelligent Material Selection
		tile[(i*4) + 2] = 0;
		tile[(i*4) + 3] = 0;
	}
	return tile;
}
