#include "stdafx.h"
#include "Building.h"
#include "GameTile.h"

/*
ContiguousList<Structure*> floors;
*/
Building::Building() {

}

//Add a structure to the stack 
//note: the structure pointer *must* stay valid
//until the last call to ConstructStructure()
void Building::Stack(Structure * floor) {
	_ASSERTE(floor != NULL);

	//For now ignore NULL floors, but this shouldn't happen
	if (floor == NULL)
		return;
	floors.push_back(floor);
}

//Construct a structure out of every floor given
void  Building::ConstructStructure() {
	Structure construction;
	int voxelsInStructure = 0;
	float currentHeight = 0;
	vec3 center;
	//Learn extents of construction
	for (auto floor : floors) 
		construction.Extents = glm::max(construction.Extents,floor->Extents);
	construction.Extents.z = currentHeight;
	//Reset current height
	currentHeight = 0;

	//Use learned extents to construct a structure with the lower left around 0,0,0 
	for (auto floor : floors) {
		//Calculate the center of the floor, and the center of the construction
		//move the floor so the two align
		vec3 floorOffset = construction.Extents * .5f - floor->Extents * .5f;
		floorOffset.z = currentHeight;
		//Copy the cells into the construction
		for (auto cell : floor->Cells) {
			StructureCell c = cell;
			c.pos += floorOffset;
			voxelsInStructure++;
			construction.Cells.push_back(c);
		}
		//Track height for next floor
		currentHeight += floor->Extents.z;
	}

	construction.OriginalVoxelCount = voxelsInStructure;

	//Save completed structure
	constructed =  construction;
}

void Building::PlaceStructure(GameTile * onTile, vec2 position) {
	//Copy the constructed structure
	Structure groundStructure = constructed;

	//First find the lowest height in the terrain the building will cover
	float lowestHeight = 500;
	for (float x = position.x; x < position.x+groundStructure.Extents.x; x++)
		for (float y = position.y; y < position.y+groundStructure.Extents.y; y++)
			lowestHeight = min((float)onTile->GetTileCell(vec2(x,y))->height,lowestHeight);

	//Next find every cell in the structure which would be below the terrain if the structure were placed at position, at lowestHeight
	//rewrite the vector by building a list of cells which are above that height
	vector<StructureCell> newCellArray;
	for (auto cell : groundStructure.Cells) {
		//Find the cut off height for this voxel's position
		float cutOffHeight = (float)onTile->GetTileCell(position+vec2(cell.pos))->height - lowestHeight;
		if (cell.pos.z > cutOffHeight)
			newCellArray.push_back(cell);
	}
	//Save the modified data into the structure
	groundStructure.Cells = newCellArray;
	groundStructure.Position = vec3(position,lowestHeight);
	//Add the structure to the tile
	onTile->Structures.push_back(groundStructure);
}