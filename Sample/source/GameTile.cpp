#include "stdafx.h"
#include "GameTile.h"
#include "lodepng.h"
#include "VoxelDrawSystem.h"
#include "GL3DProgram.h"
#include "GLTerrainProgram.h"
#include "TerrainChunk.h"
#include "TerrainChunkRenderer.h"
#include "BaseFrame.h"

#define CHUNK_COUNT ((TILE_SIZE/CHUNK_SIZE)*(TILE_SIZE/CHUNK_SIZE))

GameTile::GameTile() {
	Cells = new TileCell[TILE_SIZE*TILE_SIZE];
	chunks = new TerrainChunk*[CHUNK_COUNT];
	memset(chunks,0,CHUNK_COUNT*sizeof(TerrainChunk*));
	UseByDate = numeric_limits<double>::max();
}

GameTile::~GameTile() {
	delete [] Cells;
	delete [] chunks;
}

GameTile * GameTile::CreateGameTile(int x, int y) {
	GameTile * newTile = new GameTile();
	newTile->tile_x = x;
	newTile->tile_y = y;
	return newTile;
}

GameTile * GameTile::CreatePlaceholderTile(int x, int y) {
	GameTile * newTile = new GameTile();
	//Because you are a placeholder tiles you must prematurely trash the internal arrays
	delete [] newTile->Cells;
	//Clean up the pointers because delete[] NULL is safe
	newTile->Cells = NULL;

	newTile->tile_x = x;
	newTile->tile_y = y;
	return newTile;
}

//Load a game tile from disk
GameTile * GameTile::LoadTileFromDisk(string tileImagePath) {
	unsigned int width;
	unsigned int height;
	vector<unsigned char> tileData;

	cout << "Starting load of tile \"" << tileImagePath << "\"\n";
	//First load the tile map data from the image
	if (lodepng::decode(tileData,width,height,tileImagePath)) {
		cout << "\tImage load failed.\n";
		//Error
		return NULL;
	}
	_ASSERTE(width==TILE_SIZE);
	_ASSERTE(height==TILE_SIZE);
	cout << "\tLoaded image file, now converting to game tile data.\n";
	GameTile * tile = new GameTile();
	tile->readImageData(tileData);
	return tile; 
}

//Patch edge heights to be realistic values
void GameTile::PatchStackEdges(TileCell * cellList, int cellWidth) {
	//Patch edge stack heights to be 2 (3 height) to cover most holes
	for (int i = 0; i < cellWidth; i++) {
		cellList[i].stackHeight = 1;
		cellList[i+(cellWidth-1)*cellWidth].stackHeight = 1;
	}
	for (int i = 0; i < cellWidth; i++) {
		cellList[i*cellWidth + 0].stackHeight = 1;
		cellList[i*cellWidth + cellWidth - 1].stackHeight = 1;
	}
}


void GameTile::readImageData(const vector<unsigned char> & imageData) {
	_ASSERTE(imageData.size() == (TILE_SIZE*TILE_SIZE*4));
	for (unsigned int i = 0; i < imageData.size(); i+= 4) {
		//Load every RGBA pixel into a tile cell
		Cells[i/4].height = imageData[i+0];
		Cells[i/4].materialId = imageData[i+1];
		//Stack height must be calculated separately
		Cells[i/4].stackHeight = 0;
		Cells[i/4].cellHealth = imageData[i+2];
		Cells[i/4].cellMaxHealth = imageData[i+3];
	}

	UpdateTileSection(0,0,TILE_SIZE,TILE_SIZE);
}

//Load a game tile from memory
//pending
GameTile * GameTile::LoadCompressedTileFromMemory(const vector<unsigned char> & tileData) {
	unsigned int width;
	unsigned int height;
	vector<unsigned char> imageData;

	cout << "Starting load of tile from memory\n";
	//First load the tile map data from the image
	if (lodepng::decode(imageData,width,height,tileData)) {
		cout << "\tImage load failed.\n";
		//Error
		return NULL;
	}
	_ASSERTE(width==TILE_SIZE);
	_ASSERTE(height==TILE_SIZE);
	cout << "\tLoaded image file, now converting to game tile data.\n";
	GameTile * tile = new GameTile();
	tile->readImageData(imageData);
	return tile; 
}

void GameTile::LoadTileFromMemoryIntoExisting(const vector<unsigned char> & tileData, GameTile * newTile) {
	newTile->readImageData(tileData);
}

//Recalculate stack heights for the given region of tile cells
void GameTile::CalculateStackSizes(TileCell * cells, unsigned int width, unsigned int rx, unsigned int ry, unsigned int tox, unsigned int toy) {
	for (unsigned int y = ry; y < toy; y++) {
		for (unsigned int x = rx; x < tox; x++) {
			unsigned char lowestHeight;
			unsigned char checkHeight;
			//Your stack size is the stack necessary to 
			//cover up the hole between you and your lowest neighboring voxel
			//so first find the lowest neighboring voxel
			lowestHeight = cells[y*(int)width + (x + 1)].height;

			if ((checkHeight = cells[y*(int)width + (x - 1)].height) < lowestHeight)
				lowestHeight = checkHeight;
			if ((checkHeight = cells[(y + 1)*(int)width + x].height) < lowestHeight)
				lowestHeight = checkHeight;
			if ((checkHeight = cells[(y - 1)*(int)width + x].height) < lowestHeight)
				lowestHeight = checkHeight;
			//Now determine the stack height based off of the lowest height around you
			unsigned char myHeight = cells[y*(int)width+x].height;
			if (lowestHeight < myHeight)
				cells[y*(int)width+x].stackHeight = myHeight-lowestHeight;
			else
				cells[y*(int)width+x].stackHeight = 0;
		}
	}
}
//Recalculate stack heights for the given region of tile cells
void GameTile::UpdateTileSection(unsigned int rx, unsigned int ry, unsigned int tox, unsigned int toy) {
	bool doCalc = false;
	//If the full tile is being updated apply stack edge patching
	if ((rx == 0) && (ry == 0) && (tox == TILE_SIZE) && (toy == TILE_SIZE)) {
		PatchStackEdges(Cells,TILE_SIZE);
		doCalc = true;
	}


	//Calculate the finest grain stack sizes
	CalculateStackSizes(Cells,TILE_SIZE,rx+1,ry+1,tox-1,toy-1);

	if (chunks[0] == NULL) {
		//Generate all the chunks for this tile
		for (int y = 0; y < TILE_SIZE; y+= CHUNK_SIZE)
			for (int x = 0; x < TILE_SIZE; x+= CHUNK_SIZE) {
				int chunkx = x/CHUNK_SIZE;
				int chunky = y/CHUNK_SIZE;
				chunks[chunky*(TILE_SIZE/CHUNK_SIZE)+chunkx] = new TerrainChunk(this,x,y);
			}

	}
	else {
		//Reconstruct the chunks which are in the region specified
		//Chunk-align the tox,toy
		rx -= rx % CHUNK_SIZE;
		ry -= ry % CHUNK_SIZE;
		for (unsigned int y = ry; y < toy; y+= CHUNK_SIZE) {
			for (unsigned int x = rx;x < tox; x+= CHUNK_SIZE) {
				//Find the chunk the given x,y is in
				int chunkx = x/CHUNK_SIZE;
				int chunky = y/CHUNK_SIZE;

				//Rebuild that chunk
				chunks[chunky*(TILE_SIZE/CHUNK_SIZE)+chunkx]->Reconstruct();

			}
		}
	}

}

TileCell * GameTile::GetTileCell(vec2 pos) {
	_ASSERTE((pos.x >= 0) && (pos.y >= 0));
	_ASSERTE((pos.x < (float)TILE_SIZE) && (pos.y < (float)TILE_SIZE));
	//First lookup which tile the position is in
	int tileX = (int)floor(pos.x);
	int tileY = (int)floor(pos.y);

	return &Cells[tileY*TILE_SIZE+tileX];
}

//Carves a square crater from fx,fy to tox,toy to depth "depth" and adds all removed voxels
//to the removedVoxels value
void GameTile::Crater(IntRect craterRegion, int craterBottomZ, float damageDone, vec3 epicenter, float radius, vector<vec4> & removedVoxels) {
	_ASSERTE((craterRegion.StartX >= 0) && (craterRegion.StartY >= 0));
	_ASSERTE((craterRegion.StartX < (float)TILE_SIZE) && (craterRegion.StartY < (float)TILE_SIZE));	
	_ASSERTE((craterRegion.StartX <= craterRegion.EndX) && (craterRegion.StartY <= craterRegion.EndY));
	_ASSERTE(craterBottomZ > 0);

	int cellsRemoved = 0;

	for (int x = craterRegion.StartX; x <= craterRegion.EndX; x++) {
		for (int y = craterRegion.StartY; y <= craterRegion.EndY; y++) {
			TileCell& cell = Cells[y*TILE_SIZE+x];
			int height = cell.height;
			int heightDiff = height-craterBottomZ+1;
			//Skip where the terrain does not intersect the crater
			if (heightDiff < 0)
				continue;

			//Damage the voxel
			//determine damage scaled with distance from the epicenter
			float damageScaler = (radius - glm::distance(vec2(x,y),vec2(epicenter))/radius);
			damageScaler = max(0.1f,min(damageScaler,1.0f));
			float damage = damageDone*damageScaler;
			float originalLife = cell.cellHealth;
			float newLife = floor(originalLife-damage);
			//Only do partial destruction if the voxel isn't dead
			if (newLife > 0) {
				cell.cellHealth = (int)newLife;
				continue;
			}

			//Keep track of all removed voxels
			while (heightDiff >= 0) {
				removedVoxels.push_back(vec4(x+(tile_x*TILE_SIZE),y+(tile_y*TILE_SIZE),height,cell.materialId));
				cellsRemoved++;
				heightDiff--;
				height--;
			}
			//Expose a half health surface
			cell.cellHealth = (int)cell.cellMaxHealth/2;

			//Alter the height to create the crater
			Cells[y*TILE_SIZE+x].height = craterBottomZ-1;
		}
	}

	//Update nothing if no cells changed
	if (cellsRemoved <= 0)
		return;

	//Alter the values to be large enough for stack calculation
	craterRegion.StartX-=2;
	craterRegion.StartY-=2;
	craterRegion.EndX+=4;
	craterRegion.EndY+=4;
	//Limit the values to valid ranges
	if (craterRegion.StartX < 0)
		craterRegion.StartX = 0;
	if (craterRegion.StartY < 0)
		craterRegion.StartY = 0;
	if (craterRegion.EndX > TILE_SIZE-1)
		craterRegion.EndX = TILE_SIZE-1;
	if (craterRegion.EndY > TILE_SIZE-1)
		craterRegion.EndY = TILE_SIZE-1;
	//Rebuild the stack heights in this region
	UpdateTileSection(craterRegion.StartX,craterRegion.StartY,craterRegion.EndX,craterRegion.EndY);
}

//Recode the tile to uncompressed png data
vector<unsigned char> GameTile::recode() {
	//Convert the tile to RGBA pixel data
	vector<unsigned char> rawTileData(TILE_SIZE*TILE_SIZE*4);
	for (int y = 0; y < TILE_SIZE; y++) {
		for (int x = 0; x < TILE_SIZE; x++) {
			int cellIndex = (x+y*TILE_SIZE);
			rawTileData[cellIndex*4+0] = Cells[cellIndex].height;
			rawTileData[cellIndex*4+1] = Cells[cellIndex].materialId;
			rawTileData[cellIndex*4+2] = Cells[cellIndex].cellHealth;
			rawTileData[cellIndex*4+3] = Cells[cellIndex].cellMaxHealth;
		}
	}
	return rawTileData;
}

//Save the tile to disk
void GameTile::SaveTile(string saveName) {
	vector<unsigned char> input(recode());
	unsigned int error = lodepng::encode(saveName.c_str(),input,TILE_SIZE,TILE_SIZE);
	if (error) {
		cout << "Failed to save tile. Lodepng error " << error << ": "<< lodepng_error_text(error) << "\n";
	}
}


//Save the tile to memory
vector<unsigned char> GameTile::SaveTileToMemory() {
	vector<unsigned char> input(recode());
	vector<unsigned char> output;
	unsigned int error = lodepng::encode(output,input,TILE_SIZE,TILE_SIZE);
	if (error) {
		cout << "Failed to save tile. Lodepng error " << error << ": "<< lodepng_error_text(error) << "\n";
		return vector<unsigned char>();
	}
	return output;
}

//Render the given region using the specified detail level
//the rect should be tile-relative coordinates
void GameTile::Render(GL3DProgram * voxelShader, GLTerrainProgram * terrainShader, TerrainChunkRenderer * terrainRenderer, VoxelDrawSystem * cellDrawSystem, Polygon<4>& drawRegion, int & voxelCount)
{
	// Use the terrain shader
	terrainShader->UseProgram();
    
	// Offset the tile to the correct location
	terrainShader->Model.PushMatrix();
	terrainShader->Model.Translate(vec3(tile_x * TILE_SIZE, tile_y * TILE_SIZE, 0));
	terrainShader->Model.Apply();
    
    // we're on tile
    float x = (tile_x * TILE_SIZE);
    float y = (tile_y * TILE_SIZE);
    
	//Draw the tiles in the region specified
	for (float j = 0; j < TILE_SIZE; j += CHUNK_SIZE)
    {
		for (float i = 0; i < TILE_SIZE; i += CHUNK_SIZE)
        {
            // Calculate the world coordinate of this chunk
            vec2 t(x + i, y + j);
            
            // Calculate the polygon of the tile
            Polygon<4> region;
            region.vertices[0] = t;
            region.vertices[1] = t + vec2((float) CHUNK_SIZE, 0.0f);
            region.vertices[2] = t + vec2((float) CHUNK_SIZE, (float) CHUNK_SIZE);
            region.vertices[3] = t + vec2(0.0f, (float) CHUNK_SIZE);
            region.compute_edges();
            
            // Does this chunk intersect the viewing area
            if(drawRegion.Intersects(region))
            {
                // Compute the coordinate of the chunk and get it
                size_t cx = i / CHUNK_SIZE;
                size_t cy = j / CHUNK_SIZE;
                TerrainChunk * chunk = chunks[cy*(TILE_SIZE/CHUNK_SIZE)+cx];
                
                // Render the chunk
                terrainRenderer->RenderTerrainChunk(vec2(tile_x,tile_y)*(float)TILE_SIZE,chunk,terrainShader);
                voxelCount += chunk->VoxelCount;
            }
		}
	}
	//Undo tile translation
	terrainShader->Model.PopMatrix();
}