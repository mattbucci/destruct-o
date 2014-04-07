
#pragma once

#include "stdafx.h"

#define CHUNK_SIZE 16

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

	//A POD type of vertex
	PACK(
	struct PODVec3 {
		float x;
		float y;
		float z;
	});
	//Convert a normal vector to a POD
	static PODVec3 toPOD(vec3 vector);

	//The interleaved data structure
	//Use packing to insure everything is in contiguous memory
	//and does not have padding
	PACK(
	struct ChunkVertexData {
		PODVec3 Vertex;
		uint8_t TextureCoordinateX;
		uint8_t TextureCoordinateY;
		uint8_t TextureCoordinateSX;
		uint8_t TextureCoordinateSY;
	});

	//Array of interleaved vertex data
	ChunkVertexData * VertexData;
	int VertexDataSize;
};