#include "stdafx.h"
#include "VoxelSystem.h"
#include "lodepng.h"
#include "TerrainGen.h"

#include "GL3DProgram.h"
#include "OS.h"

#include "GameTile.h"

#include "VoxelDrawSystem.h"
#include "InstancedVoxelRenderSystem.h"
#include "BasicVoxelRenderSystem.h"
#include <sstream>
#include <thread>

VoxelSystem::VoxelSystem() {
	cellRenderer = VoxelDrawSystem::BuildAppropriateSystem();
	tiles = new TileHandler();
	tiles->init();

	//Just get the starting tile
	tiles->getTile(vec2(0, 0));

	//Load the tile textures
	unsigned int textureWidth, textureHeight;
	vector<unsigned char> image;

	//A smart system would have multiple res tiles and would automatically select
	//one appropriate for the system its running on
	lodepng::decode(image, textureWidth, textureHeight, "terrain/tiles-lowres.png");

	//I should have moved the png->texture into a utility library
	//later...
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth,
		textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		&image[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//GL_NEAREST FOR SPEED
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_NEAREST FOR SPEED
}
VoxelSystem::~VoxelSystem() {
	delete cellRenderer;
}

GameTile * VoxelSystem::GetTile(vec2 pos) {
	//TileHandler Guaranteed to Return Tile
	return tiles->getTile(pos);
}

TileCell * VoxelSystem::GetTileCellAt(vec2 pos) {
	GameTile * tileData = GetTile(vec2(floor(pos.x / TILE_SIZE), floor(pos.y / TILE_SIZE)));

	//Convert to relative position
	pos.x -= tileData->tile_x*TILE_SIZE;
	pos.y -= tileData->tile_y*TILE_SIZE;

	if ((pos.x < 0) || (pos.y < 0))
		return NULL;
	
	if ((pos.x >= TILE_SIZE) || (pos.y >= TILE_SIZE))
		return NULL;

	return tileData->GetTileCell(pos);
}

//When the voxel system calculates position height
//it will have to check which tile the position is in
//in the future. For now only one tile is loaded
float VoxelSystem::GetPositionHeight(vec2 pos) {
	TileCell * cell = GetTileCellAt(pos);

	_ASSERTE(cell != NULL);
	return cell->height + 1.0f;
}

//Get the stack size of a specific position
//all positions acceptable
int VoxelSystem::GetPositionStackSize(vec2 pos) {
	TileCell * cell = GetTileCellAt(pos);

	if (cell == NULL)
		return -1;

	//Player is within the set of valid tiles

	return cell->stackHeight + 1;
}

void VoxelSystem::forTilesInRect(IntRect region, function<void(GameTile * tile)> foreachFunction) {
	for (float x = region.StartX;; x+=TILE_SIZE) {
		for (float y = region.StartY;; y+=TILE_SIZE) {
			//Retrieve a valid tile for this position
			//and call the foreachFunction
			foreachFunction(GetTile(glm::floor(vec2(x/TILE_SIZE,y/TILE_SIZE))));

			//Exit only after an iteration past the end
			if (y >= region.EndY)
				break;
		}
		//Exit only after an iteration past the end
		if (x >= region.EndX)
			break;
	}
}

//intersects two ranges
//places the output into rangeA
static void intersect1D(int & rangeAStart, int & rangeAEnd, int rangeBStart, int rangeBEnd) {
	//No intersection
	if ((rangeAStart > rangeBEnd) || (rangeAEnd < rangeBStart)) {
		rangeAEnd = rangeAStart;
		return;
	}
	//Find the range intersection
	rangeAStart = max(rangeAStart, rangeBStart);
	rangeAEnd = min(rangeAEnd, rangeBEnd);
}

struct rectGroup {
	IntRect parts[4];
	int usedParts;
};
static rectGroup subtract(IntRect outer, IntRect inner) {
	//If the inner region is empty, the outer region is the intersection
	if ((inner.StartX == inner.EndX) || (inner.StartY == inner.EndY)) {
		rectGroup grp;
		grp.usedParts = 1;
		grp.parts[0] = outer;
		return grp;
	}
	//If the inner region is not empty there are four regions that could describe the intersection
	rectGroup grp;
	grp.usedParts = 0;
	if (outer.StartY < inner.StartY)
		grp.parts[grp.usedParts++] = IntRect(outer.StartX,outer.StartY,outer.EndX,inner.StartY);
	if (outer.StartX < inner.StartX) 
		grp.parts[grp.usedParts++] = IntRect(outer.StartX,inner.StartY,inner.StartX,inner.EndY);
	if (outer.EndX > inner.EndX) 
		grp.parts[grp.usedParts++] = IntRect(inner.EndX,inner.StartY,outer.EndX,inner.EndY);
	if (outer.EndY > inner.EndY) 
		grp.parts[grp.usedParts++] = IntRect(outer.StartX,inner.EndY,outer.EndX,outer.EndY);
	return grp;
};

//Draw the voxels in a region
void VoxelSystem::Draw(GL3DProgram * shader, vec3 drawPos, IntRect drawRegion) {
	//_ASSERTE(tileData != NULL);

	//Enable voxel texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);

	voxelCount = 0;


	//render each viewable rectangle
	terrainRenderer.StartRendering(shader);

	forTilesInRect(drawRegion,[this,shader,drawRegion](GameTile * tile) {
		GameTile & current_tile = *tile;
		//Get the region this tile is in
		IntRect tileRegion(current_tile.tile_x * TILE_SIZE,current_tile.tile_y * TILE_SIZE,
			current_tile.tile_x * TILE_SIZE + TILE_SIZE,current_tile.tile_y * TILE_SIZE + TILE_SIZE);

		//Intersect it with the regions you're supposed to be drawing
		IntRect tileDrawRegion = drawRegion;
		tileDrawRegion.Intersect(tileRegion);
		//medium.Intersect(tileRegion);
		//low.Intersect(tileRegion);


		//Now offset the regions by the tile position so that it is relative to the tile
		vec2 tileOffset = vec2(current_tile.tile_x * TILE_SIZE,current_tile.tile_y * TILE_SIZE);
		tileDrawRegion -= tileOffset;
		
		//Skip zero length segments
		//Render the given region of the tile in high detail
		current_tile.Render(shader,&terrainRenderer,cellRenderer,tileDrawRegion,voxelCount);


	});

	terrainRenderer.FinishRendering(shader);

}

void VoxelSystem::Update(vec3 player_pos){
	//Pass to TileHandler
	tiles->getTile(vec2(floor(player_pos.x / TILE_SIZE), floor(player_pos.y / TILE_SIZE)));
}

int VoxelSystem::GetLastVoxelCount() {
	return voxelCount;
}

void VoxelSystem::Paint(vec2 pos, int newMaterial) {
	TileCell * cell = GetTileCellAt(pos);

	_ASSERTE(cell != NULL);

	//Player is within the set of valid tiles
	//Paint the tile material
	cell->materialId = newMaterial;
}

//Deforms a region of voxels, punching a crater into the given position
//all voxels removed are returned as positions
vector<vec4> VoxelSystem::Crater(vec3 pos, float size) {
	//render each viewable rectangle
	vector<vec4> removedVoxels;
	forTilesInRect(IntRect(pos.x - size / 2,pos.y - size / 2,pos.x + size / 2,pos.y + size / 2),[this,&removedVoxels,pos,size](GameTile * current_tile) {
		int rectStartX, rectStartY, rectEndX, rectEndY;
		//Get the region this tile is in
		rectStartX = current_tile->tile_x * TILE_SIZE;
		rectStartY = current_tile->tile_y * TILE_SIZE;
		rectEndX = rectStartX + TILE_SIZE-1;
		rectEndY = rectStartY + TILE_SIZE-1;
		//Intersect it with the region you're supposed to be drawing
		intersect1D(rectStartX, rectEndX, pos.x - size / 2, pos.x + size / 2);
		intersect1D(rectStartY, rectEndY, pos.y - size / 2, pos.y + size / 2);


		//Now offset the region by the tile position so that it is relative to the tile
		rectStartX -= current_tile->tile_x * TILE_SIZE;
		rectStartY -= current_tile->tile_y * TILE_SIZE;
		rectEndX -= current_tile->tile_x * TILE_SIZE;
		rectEndY -= current_tile->tile_y * TILE_SIZE;

		//Skip zero length segments
		if ((rectStartY == rectEndY) || (rectStartX == rectEndX))
			return;

		current_tile->Crater(rectStartX, rectStartY, rectEndX, rectEndY, (int)(pos.z - size / 2.0), removedVoxels);

	});
	//Return all removed voxels
	return removedVoxels;
}


