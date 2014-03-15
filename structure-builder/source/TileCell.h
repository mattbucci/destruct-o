
#pragma once


struct TileCell {
	//Z height of the cell
	unsigned char height;
	//Number of voxels stacked BELOW this one
	//zero means no voxels below this one (but this one still exists)
	unsigned char stackHeight;
	//The material id of the cell
	unsigned char materialId;
};