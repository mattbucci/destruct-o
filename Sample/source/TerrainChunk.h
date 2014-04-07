
#pragma once

#include "stdafx.h"

#define CHUNK_SIZE 16

class GameTile;

class TerrainChunk {
	GameTile * owner;

	bool f(int i, int j, int k);
public:
	TerrainChunk(GameTile * owningTile, int chunkx, int chunky);
	~TerrainChunk();

	//Terrain chunks are relative to the tile that owns them
	float X;
	float Y;

	//The count of every individual voxel in this chunk
	//used strictly for debugging
	int VoxelCount;

	//Reconstruct the vertex array using the owned game tile
	void Reconstruct();

	//The interleaved data structure
	struct ChunkVertexData {
		vec3 Vertex;
		uint8_t TextureCoordinate;
	};

	//Array of interleaved vertex data
	ChunkVertexData * VertexData;
	int VertexDataSize;
};