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
#include "SimplePolygon.h"

#include "BaseFrame.h"
#include "GLTexture.h"

VoxelSystem::VoxelSystem()
{
    cellRenderer = VoxelDrawSystem::BuildAppropriateSystem();
}
VoxelSystem::~VoxelSystem() {
	delete cellRenderer;
}

GameTile * VoxelSystem::GetTile(vec2 pos) {
	//TileHandler Guaranteed to Return Tile
	return tiles.getTile(vec2i((int)pos.x,(int)pos.y));
}

TileCell * VoxelSystem::GetTileCellAt(vec2 pos) {
	GameTile * tileData = GetTile(vec2(floor(pos.x / TILE_SIZE), floor(pos.y / TILE_SIZE)));

	//Floor here to eliminate occasional rounding error
	pos = glm::floor(pos);

	//Convert to relative position
	pos.x -= tileData->tile_x*TILE_SIZE;
	pos.y -= tileData->tile_y*TILE_SIZE;

	//ensure basic assumptions
	_ASSERTE ((pos.x >= 0) && (pos.y >= 0));
	_ASSERTE ((pos.x < TILE_SIZE) && (pos.y < TILE_SIZE));

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

	region.StartX = (int)floor((float)region.StartX/(float)TILE_SIZE)*TILE_SIZE;
	region.StartY = (int)floor((float)region.StartY/(float)TILE_SIZE)*TILE_SIZE;
	for (float x = region.StartX; x < region.EndX; x+=(float)TILE_SIZE) {
		for (float y = region.StartY; y < region.EndY; y+=(float)TILE_SIZE) {
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

// Draw the voxels in the triangular region
void VoxelSystem::Draw(ShaderGroup * shaders, SimplePolygon<4>& drawRegion)
{
    // Get the bounding box of the viewing area
    SimplePolygon<4> drawRegionBoundingBox;
    drawRegion.BoundingRect(drawRegionBoundingBox);
    
    // Compute the bounds of the tile set we need to examine
    float imin = floor(drawRegionBoundingBox.vertices[0].x / (float) TILE_SIZE) * (float) TILE_SIZE;
    float imax = ceil(drawRegionBoundingBox.vertices[2].x / (float) TILE_SIZE) * (float) TILE_SIZE;
    float jmin = floor(drawRegionBoundingBox.vertices[0].y / (float) TILE_SIZE) * (float) TILE_SIZE;
    float jmax = ceil(drawRegionBoundingBox.vertices[2].y / (float) TILE_SIZE) * (float) TILE_SIZE;
    
	// Enable voxel texture
	glActiveTexture(GL_TEXTURE0);
	Game()->Textures.GetTexture<GLTexture>("terrain/tiles-lowres.png")->Bind();
    
    // Get the shaders we need
	GLTerrainProgram * terrainShader = (GLTerrainProgram*)shaders->GetShader("terrain");
	GL3DProgram * voxelShader = (GL3DProgram*)shaders->GetShader("3d");
    
	// Begin the rendering context
	terrainRenderer.StartRendering(terrainShader);
    
    // Voxel count is zero
    voxelCount = 0;
    
    // Look at all the tiles 
    for(float j = jmin; j <= jmax; j += (float) TILE_SIZE)
    {
        for(float i = imin; i <= imax; i += (float) TILE_SIZE)
        {
            // Calculate the tile world origin and tile index
            vec2 t(i, j);
            vec2 tc = glm::floor(t / (float) TILE_SIZE);
            
            // Calculate the SimplePolygon of the tile
            SimplePolygon<4> region;
            region.vertices[0] = t;
            region.vertices[1] = t + vec2((float) TILE_SIZE, 0.0f);
            region.vertices[2] = t + vec2((float) TILE_SIZE, (float) TILE_SIZE);
            region.vertices[3] = t + vec2(0.0f, (float) TILE_SIZE);
            region.compute_edges();
            
            // If this tile is in the visible set
            if(drawRegion.Intersects(region))
            {
                // Get the tile that the viewing area has intersected
                GameTile *tile = GetTile(tc);
                
                // Render this tile with the visible region
                tile->Render(voxelShader, terrainShader, &terrainRenderer, cellRenderer, drawRegion, voxelCount);
            }
        }
    }
    
    // Finish the terrain rendering context
	terrainRenderer.FinishRendering(terrainShader);
}

void VoxelSystem::Update(vec3 player_pos){
	//Pass to TileHandler
	tiles.Update(vec2(player_pos));
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
	int craterBottom = (int)(pos.z - size / 2.0);
	//Limit the bottom of the crater
	craterBottom = max(craterBottom,2);

	//render each viewable rectangle
	vector<vec4> removedVoxels;
	forTilesInRect(IntRect(pos.x - size / 2,pos.y - size / 2,pos.x + size / 2,pos.y + size / 2),[this,craterBottom,&removedVoxels,pos,size,damage](GameTile * current_tile) {
		int rectStartX, rectStartY, rectEndX, rectEndY;
		//Get the region this tile is in
		rectStartX = current_tile->tile_x * TILE_SIZE;
		rectStartY = current_tile->tile_y * TILE_SIZE;
		rectEndX = rectStartX + TILE_SIZE-1;
		rectEndY = rectStartY + TILE_SIZE-1;
		//Intersect it with the region you're supposed to be drawing
		intersect1D(rectStartX, rectEndX, (int)(pos.x - size / 2), (int)(pos.x + size / 2));
		intersect1D(rectStartY, rectEndY, (int)(pos.y - size / 2), (int)(pos.y + size / 2));


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

		current_tile->Crater(IntRect(rectStartX, rectStartY, rectEndX, rectEndY), craterBottom,damage,vec3(expCenter,pos.z),size /2.0f, removedVoxels);

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


void VoxelSystem::NewWorld(int seed) {
	tiles.setSeed(seed);
	//Predict tiles around the starting position
	tiles.UpdateSync(vec2(),2);
}
