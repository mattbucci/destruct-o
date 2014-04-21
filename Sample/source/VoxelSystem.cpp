#include "stdafx.h"
#include "VoxelSystem.h"
#include "lodepng.h"

#include "GL3DProgram.h"
#include "OS.h"

#include "GameTile.h"

#include "VoxelDrawSystem.h"
#include "InstancedVoxelRenderSystem.h"
#include "BasicVoxelRenderSystem.h"
#include "ShaderGroup.h"

#include "PhysicsUtilities.h"


VoxelSystem::VoxelSystem() {
	cellRenderer = VoxelDrawSystem::BuildAppropriateSystem();

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
	return tiles.getTile(pos);
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
	//Align the starting position with the nearest tile boundary

	region.StartX = floor((float)region.StartX/(float)TILE_SIZE)*TILE_SIZE;
	region.StartY = floor((float)region.StartY/(float)TILE_SIZE)*TILE_SIZE;
	for (float x = region.StartX; x < region.EndX; x+=TILE_SIZE) {
		for (float y = region.StartY; y < region.EndY; y+=TILE_SIZE) {
			//Retrieve a valid tile for this position
			//and call the foreachFunction
			foreachFunction(GetTile(glm::floor(vec2(x/TILE_SIZE,y/TILE_SIZE))));
		}
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
void VoxelSystem::Draw(ShaderGroup * shaders, vec3 drawPos, IntRect drawRegion) {
	//_ASSERTE(tileData != NULL);

	//Enable voxel texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);

	voxelCount = 0;

	GLTerrainProgram * terrainShader = (GLTerrainProgram*)shaders->GetShader("terrain");
	GL3DProgram * voxelShader = (GL3DProgram*)shaders->GetShader("3d");


	//render each viewable rectangle
	terrainRenderer.StartRendering(terrainShader);

	forTilesInRect(drawRegion,[this,terrainShader,voxelShader,drawRegion](GameTile * tile) {
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
		current_tile.Render(voxelShader,terrainShader,&terrainRenderer,cellRenderer,tileDrawRegion,voxelCount);


	});

	terrainRenderer.FinishRendering(terrainShader);

}

void VoxelSystem::Update(vec3 player_pos){
	//Pass to TileHandler
	tiles.getTile(vec2(floor(player_pos.x / TILE_SIZE), floor(player_pos.y / TILE_SIZE)));
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
vector<vec4> VoxelSystem::Crater(vec3 pos, float size, float damage) {
	//render each viewable rectangle
	vector<vec4> removedVoxels;
	forTilesInRect(IntRect(pos.x - size / 2,pos.y - size / 2,pos.x + size / 2,pos.y + size / 2),[this,&removedVoxels,pos,size,damage](GameTile * current_tile) {
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

		//adjust the explosion center for the tile
		vec2 expCenter = vec2(pos)-(vec2(current_tile->tile_x,current_tile->tile_y) * (float)TILE_SIZE);

		current_tile->Crater(IntRect(rectStartX, rectStartY, rectEndX, rectEndY), (int)(pos.z - size / 2.0),damage,vec3(expCenter,pos.z), removedVoxels);

	});
	//Return all removed voxels
	return removedVoxels;
}


bool VoxelSystem::RaytraceToTerrain(vec3 from, vec3 direction, float & rayLength, vec3 & surfaceNormal) {
	//Ray trace in 2d to get a short list of possible colliding voxels from the terrain
	vec2 p0 = vec2(from);
	//Hits surfaces up to 200 away
	vec2 p1 = vec2(from+direction*200.0f);

	//2d ray tracing adapted from
	//http://playtechs.blogspot.com/2007/03/raytracing-on-grid.html
	vec2 d = glm::abs(p1-p0);

	int x = int(floor(p0.x));
	int y = int(floor(p0.y));

	int n = 1;
	int x_inc, y_inc;
	float error;

	if (d.x == 0) {
		x_inc = 0;
		error = std::numeric_limits<float>::infinity();
	}
	else if (p1.x > p0.x) {
		x_inc = 1;
		n += int(floor(p1.x)) - x;
		error = (floor(p0.x) + 1 - p0.x) * d.y;
	}
	else {
		x_inc = -1;
		n += x - int(floor(p1.x));
		error = (p0.x - floor(p0.x)) * d.y;
	}

	if (d.y == 0) {
		y_inc = 0;
		error -= std::numeric_limits<float>::infinity();
	}
	else if (p1.y > p0.y) {
		y_inc = 1;
		n += int(floor(p1.y)) - y;
		error -= (floor(p0.y) + 1 - p0.y) * d.x;
	}
	else {
		y_inc = -1;
		n += y - int(floor(p1.y));
		error -= (p0.y - floor(p0.y)) * d.x;
	}

	for (; n > 0; --n) {
		//Paint it to mark it as visited
		//voxelSystem->Paint(vec2(x,y),5);
		//Check the voxel for a ray collision in 3d space
		//Check every voxel that's in this 2d region
		float height = GetPositionHeight(vec2(x,y));
		int stackSize = GetPositionStackSize(vec2(x,y));
		for (int stack = 0; stack <= stackSize; stack++) {
			if (PhysicsUtilities::TraceToVoxel(from,direction,vec3(x,y,height-(float)stack),rayLength,surfaceNormal))
				//Collided with terrain
				return true;
		}


		//Move to the next voxel
		if (error > 0) {
			y += y_inc;
			error -= d.x;
		}
		else {
			x += x_inc;
			error += d.y;
		}
	}

	//No collision found
	return false;
}
