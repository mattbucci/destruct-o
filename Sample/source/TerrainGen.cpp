#include "stdafx.h"
#include <noise/noise.h>
#include "noiseutils.h"
#include "TerrainGen.h"

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

	//Set up Blend Level 3
	blend3Blender.SetSourceModule(0, blend2Blender);
	blend3Blender.SetSourceModule(1, detailNoise);

	//Default Scale to 1
	tilesx = 4;
	tilesy = 4;

	//Default Tile Size to 0 (Invalid Tile Size)
	tilex = 0;
	tiley = 0;

	//Seed with -1 (Invalid Seed Marker)
	setSeed(-1);
}
TerrainGen::~TerrainGen() {
	//Nothing to Free
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
}

int TerrainGen::BufferIndex(int x, int y) {
	return 4 * (y*tiley + x);
}

void TerrainGen::generateTerrain(GameTile * tile) {
	//Ensure Tile Size Selected
	_ASSERTE(tilex != 0 && tiley != 0);
	//Ensure Seed Changed
	_ASSERTE(getSeed() != -1);

	//Calculate Tile Bounds
	double bx0 = tile->tile_x * tilesx;
	double bx1 = bx0 + tilesx;
	double by0 = tile->tile_y * tilesx;
	double by1 = by0 + tilesy;

	//Create Local HeightMap and HeightMapBuilder
	noise::utils::NoiseMap heightMap;
	noise::utils::NoiseMapBuilderPlane heightMapBuilder;

	//Initialize HeightmapBuilder
	heightMapBuilder.SetDestSize(tilex, tiley);
	heightMapBuilder.SetSourceModule(blend3Blender);
	heightMapBuilder.SetDestNoiseMap(heightMap);
	heightMapBuilder.SetBounds(bx0, bx1, by0, by1);
	//Build Tile
	heightMapBuilder.Build();

	//Get Pointer to Raw Terrain Data
	float* rawTerrain = heightMap.GetSlabPtr();

	//Allocate Space for Compiled Tile
	int s = tilex * tiley;
	unsigned char* rawtile = new unsigned char[s * 4];

	//Compile Tile Data
	for(int i = 0; i < s; i++) {
		if ((unsigned char)(rawTerrain[i] * 86 + 128)>1)
			rawtile[(i * 4)] = (unsigned char)(rawTerrain[i] * 86 + 128);
		else
			rawtile[(i * 4)] = 1;
		rawtile[(i*4) + 1] = 2; // TODO: Intelligent Material Selection
		rawtile[(i*4) + 2] = 0;
		rawtile[(i*4) + 3] = 0;
	}

	//Assign Data to Container
	vector<unsigned char> tilecontainer;
	tilecontainer.assign(rawtile, rawtile + (tilex * tiley * 4));
	//TODO Determine tile type based on terrain properties
	//Free Generatored Terrain Data
	delete rawtile;

	//Load Tile Data into GameTile
	GameTile::LoadTileFromMemoryIntoExisting(tilecontainer, tile);
}
