
#pragma once

#include "stdafx.h"

#include "Utilities.h"

#define CHUNK_SIZE 32

class GameTile;

class TerrainChunk {
	GameTile * owner;

	//Check if the given ijk contains a voxel
	bool f(int i, int j, int k);
	//check if the given position is beneath the surface
	bool fu(int i, int j, int k);
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

	//If this chunk was modified, this is set to true
	//when first rendered becomes false
	bool Dirty;



	//The interleaved data structure
	//Use packing to insure everything is in contiguous memory
	//and does not have padding

	struct ChunkVertexData {
		Utilities::PODVec3 Vertex;
		uint8_t TextureCoordinateX;
		uint8_t TextureCoordinateY;
		uint16_t Material;
	};

	//Array of interleaved vertex data
	ChunkVertexData * VertexData;
    GLushort * VertexIndices;
	size_t VertexDataSize;
    size_t VertexIndicesSize;
};