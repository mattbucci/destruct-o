#pragma once

#include "stdafx.h"
#include "GLDynamicVertexGroup.h"

class GL3DProgram;

class VoxelSystem : GLDynamicVertexGroup {
	unsigned int tileWidth;
	unsigned int tileHeight;
	//Stores the tile in RGBA format 
	vector<unsigned char> rawTile;
	//Push a side of the cube
	void pushSide(vec3 pos, vec3 normal, vec3 a, vec3 b, vec3 c, vec3 d, int & vertNumber);
	//Push a voxel into the glydynamicvertexgroup starting at vertnumber
	void pushVoxel(vec3 pos,int materialId, int & vertNumber);
public:
	VoxelSystem();
	~VoxelSystem();

	//Attempt to load a tile from disc
	bool LoadTile(string tileName);

	//Draw the voxels in a region
	//atx,aty and tox, toy define two corners of the rectangle
	//of voxels which will be rendered
	void Draw(GL3DProgram * shader, vec3 drawPos, int atx, int aty, int tox, int toy);

	//Get map width
	int GetWidth();
	//Get map height
	int GetHeight();
};