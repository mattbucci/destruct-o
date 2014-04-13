#include "stdafx.h"
#include "GameTile.h"
#include "lodepng.h"
#include "VoxelDrawSystem.h"
#include "GL3DProgram.h"
#include "GLTerrainProgram.h"
#include "TerrainChunk.h"
#include "TerrainChunkRenderer.h"

#define CHUNK_COUNT ((TILE_SIZE/CHUNK_SIZE)*(TILE_SIZE/CHUNK_SIZE))

GameTile::GameTile() {
	Cells = new TileCell[TILE_SIZE*TILE_SIZE];
	chunks = new TerrainChunk*[CHUNK_COUNT];
	memset(chunks,0,CHUNK_COUNT*sizeof(TerrainChunk*));
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
	return LoadTileFromMemory(tileData);
}

//Patch edge heights to be realistic values
void GameTile::PatchStackEdges(TileCell * cellList, int cellWidth) {
	//Patch edge stack heights to be 2 (3 height) to cover most holes
	for (unsigned int i = 0; i < cellWidth; i++) {
		cellList[i].stackHeight = 1;
		cellList[i+(cellWidth-1)*cellWidth].stackHeight = 1;
	}
	for (unsigned int i = 0; i < cellWidth; i++) {
		cellList[i*cellWidth + 0].stackHeight = 1;
		cellList[i*cellWidth + cellWidth - 1].stackHeight = 1;
	}
}




//Load a game tile from memory
//pending
GameTile * GameTile::LoadTileFromMemory(const vector<unsigned char> & tileData) {
	_ASSERTE(tileData.size() == (TILE_SIZE*TILE_SIZE*4));
	cout << "Starting Load of tile from memory\n";
	GameTile * tile = new GameTile();
	for (unsigned int i = 0; i < tileData.size(); i+= 4) {
		//Load every RGBA pixel into a tile cell
		tile->Cells[i/4].height = tileData[i+0];
		tile->Cells[i/4].materialId = tileData[i+1];
		//Stack height must be calculated separately
		tile->Cells[i/4].stackHeight = 0;
	}

	cout << "\tLoaded game tile data. Now building stacks.\n";
	tile->UpdateTileSection(0,0,TILE_SIZE,TILE_SIZE);

	cout << "\tTile Load Complete.\n";
	return tile;
}

void GameTile::LoadTileFromMemoryIntoExisting(const vector<unsigned char> & tileData, GameTile * newTile) {
	_ASSERTE(tileData.size() == (TILE_SIZE * TILE_SIZE * 4));

	for (unsigned int i = 0; i < tileData.size(); i+= 4) {
		newTile->Cells[i/4].height = tileData[i+0];
		newTile->Cells[i/4].materialId = tileData[i+1];
		newTile->Cells[i/4].stackHeight = 0;
	}

	newTile->UpdateTileSection(0,0,TILE_SIZE,TILE_SIZE);
	for (int i = 0; i < TILE_SIZE; i++) {
		newTile->Cells[i].stackHeight = 2;
		newTile->Cells[i+(TILE_SIZE-1)*TILE_SIZE].stackHeight = 2;
	}
	for (int i = 0; i < TILE_SIZE; i++) {
		newTile->Cells[i*TILE_SIZE + 0].stackHeight = 2;
		newTile->Cells[i*TILE_SIZE + TILE_SIZE - 1].stackHeight = 2;
	}
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
		for (int y = ry; y < toy; y+= CHUNK_SIZE) {
			for (int x = rx;x < tox; x+= CHUNK_SIZE) {
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
void GameTile::Crater(int fx, int fy, int tox, int toy, int craterBottomZ, vector<vec4> & removedVoxels) {
	_ASSERTE((fx >= 0) && (fy >= 0));
	_ASSERTE((tox < (float)TILE_SIZE) && (toy < (float)TILE_SIZE));	
	_ASSERTE((fx <= tox) && (fy <= toy));
	_ASSERTE(craterBottomZ > 0);


	for (int x = fx; x <= tox; x++) {
		for (int y = fy; y <= toy; y++) {
			TileCell& cell = Cells[y*TILE_SIZE+x];
			int height = cell.height;
			int heightDiff = height-craterBottomZ;
			//Skip where the terrain does not intersect the crater
			if (heightDiff < 0)
				break;
			//Keep track of all removed voxels
			while (heightDiff >= 0) {
				removedVoxels.push_back(vec4(x+(tile_x*TILE_SIZE),y+(tile_y*TILE_SIZE),height,cell.materialId));
				heightDiff--;
				height--;
			}
			//Alter the height to create the crater
			Cells[y*TILE_SIZE+x].height = craterBottomZ-1;
		}
	}
	//Alter the values to be large enough for stack calculation
	fx-=2;
	fy-=2;
	tox+=4;
	toy+=4;
	//Limit the values to valid ranges
	if (fx < 1)
		fx = 1;
	if (fy < 1)
		fy = 1;
	if (tox > TILE_SIZE-1)
		tox = TILE_SIZE-1;
	if (toy > TILE_SIZE-1)
		toy = TILE_SIZE-1;
	//Rebuild the stack heights in this region
	UpdateTileSection(fx,fy,tox,toy);
}

//Save the tile to disk
void GameTile::SaveTile(string saveName) {
	//Convert the tile to RGBA pixel data
	vector<unsigned char> rawTileData(TILE_SIZE*TILE_SIZE*4);
	for (int y = 0; y < TILE_SIZE; y++) {
		for (int x = 0; x < TILE_SIZE; x++) {
			int cellIndex = (x+y*TILE_SIZE);
			rawTileData[cellIndex*4+0] = Cells[cellIndex].height;
			rawTileData[cellIndex*4+1] = Cells[cellIndex].materialId;
			rawTileData[cellIndex*4+2] = 0;
			rawTileData[cellIndex*4+3] = 0;
		}
	}
	unsigned int error = lodepng::encode(saveName.c_str(),rawTileData,TILE_SIZE,TILE_SIZE);
	if (error) {
		cout << "Failed to save tile. Lodepng error " << error << ": "<< lodepng_error_text(error) << "\n";
	}
}

//Render the given region using the specified detail level
//the rect should be tile-relative coordinates
void GameTile::Render(GL3DProgram * voxelShader, GLTerrainProgram * terrainShader, TerrainChunkRenderer * terrainRenderer, VoxelDrawSystem * cellDrawSystem, IntRect drawRegion, int & voxelCount) {
	TileCell * cells;

	_ASSERTE(TILE_SIZE >= drawRegion.EndX);
	_ASSERTE(TILE_SIZE >= drawRegion.EndY);

	terrainShader->UseProgram();

	//offset rendering for tile location
	terrainShader->Model.PushMatrix();
	terrainShader->Model.Translate(vec3(tile_x * TILE_SIZE, tile_y * TILE_SIZE, 0));
	//Scale for level of detail requested
	terrainShader->Model.Apply();

	//Draw the tiles in the region specified
	for (int ry = drawRegion.StartY; ry < drawRegion.EndY; ry+= CHUNK_SIZE) {
		for (int rx = drawRegion.StartX;rx < drawRegion.EndX; rx+= CHUNK_SIZE) {
			//Find the chunk the given x,y is in
			int chunkx = rx/CHUNK_SIZE;
			int chunky = ry/CHUNK_SIZE;

			//Draw that chunk
			TerrainChunk * chunk = chunks[chunky*(TILE_SIZE/CHUNK_SIZE)+chunkx];
			terrainRenderer->RenderTerrainChunk(vec2(tile_x,tile_y)*(float)TILE_SIZE,chunk,terrainShader);
			voxelCount += chunk->VoxelCount;
		}
	}

	/*
	//For now only render structures in DETAIL_HIGH
	//should render boxes in other detail levels
	if (detail == DETAIL_HIGH) {
	//structures are large but we only measure using their corner, offset by the max building size to correct
	rect.StartX -= 20;
	rect.StartY -= 20;
	rect.EndX += 20;
	rect.EndY += 20;

	//Next check for any structures on this tile which intersect the view rectangle
	for (auto structure : Structures) {
	int structx = (int)structure.Position.x;
	int structy = (int)structure.Position.y;
	int structex = structx + (int)structure.Extents.x;
	int structey = structy + (int)structure.Extents.y;
	//Now see if the struct rectangle intersects the draw rectangle

	if (((rect.StartX < structx) &&
	(rect.EndX > structex))	 &&
	((rect.StartY < structy) &&
	(rect.EndY > structey))) {
	//Time to draw the structure
	//Push the structure's position
	if (structure.Cells.size() > 0){
	shader->Model.PushMatrix();
	shader->Model.Translate(structure.Position);
	shader->Model.Apply();
	//Track voxels drawn for debug
	//Todo: fix broken voxel count
	//voxelCount += structure.Cells.size();
	//Start the draw cycle
	renderer->startDraw(shader);
	StructureCell * celliterator = &structure.Cells.front();
	unsigned int endCount = structure.Cells.size();
	//Push all the cells
	for (unsigned int i = 0; i < endCount; i++, celliterator++)
	renderer->pushVoxel(shader, celliterator->pos, celliterator->material);
	//Finish drawing and remove the structure matrix
	renderer->finishDraw(shader);
	shader->Model.PopMatrix();
	}
	}
	}	
	}*/
	//Undo tile translation
	terrainShader->Model.PopMatrix();
}