#include "stdafx.h"
#include "Building.h"
#include "GameTile.h"
#include "Utilities.h"

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

	//Recenter the building and learn extents
	vec3 minVoxel = vec3(10000,10000,10000);
	vec3 maxVoxel = vec3(-10000,-10000,-10000);
	for (auto vox : groundStructure.Cells) {
		minVoxel = glm::min(vox.pos,minVoxel);
		maxVoxel = glm::max(vox.pos,maxVoxel);
	}
	//Apply recenter
	for (auto vox : groundStructure.Cells) {
		vox.pos -= minVoxel;
	}
	groundStructure.Extents = maxVoxel-minVoxel;


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



//If instead you wish to build a spire instead of a stack call this
void Building::ConstructSpire(vec3 growDirection, float bottomRadius, float length) {
	this->growDirection = growDirection;
	this->bottomRadius = bottomRadius;
	this->length = length;

	//Vs. straight up
	vec3 straightUp(0,0,1);
	float angle;
	mat4 rotMatrix;

	if (growDirection != straightUp) {
		vec3 ortho = glm::cross(straightUp,growDirection);
		ortho = glm::normalize(ortho);
		angle = acos(glm::dot(straightUp,growDirection));
		//Construct rotation matrix
		rotMatrix = glm::rotate((float)(angle/M_PI*180.0f),ortho);
	}

	
	//Place 4 voxels for each length
	for (float z = 0; z < length; z+=.5f) {
		float t = (1.0f-z/length);
		int iterations = (int)(t*length*.6f);
		float angle = (float)M_PI*2.0f/(float)iterations;
		float radius = t*bottomRadius;

		for (float r = 0; r < iterations; r+=Utilities::random(angle/2.0f,angle)) {
			float thisAngle = angle*r;
			float thisRadius = radius*Utilities::random(.5f,1.5f);
			vec3 dirVec = vec3(cos(thisAngle)*thisRadius,sin(thisAngle)*thisRadius,z);
			vec3 finalPos = vec3(rotMatrix * vec4(dirVec,1.0));
			//Find voxel position
			vec3 voxPos = glm::floor(finalPos);
			if (placed.find(voxPos) != placed.end())
				continue;

			//Add to appropriate lists
			placed.insert(voxPos);
			//Randomly blacklist 10% of voxels
			if (Utilities::random(0.0f,1.0f) >= .10f) {
				StructureCell cell;
				cell.material = 0;
				cell.pos = voxPos;
				constructed.Cells.push_back(cell);
			}

		}
	}

}
/*
void Building::addNeighbor(vec3 neighbor) {
	//First add to appropriate lists
	neighbors.insert(neighbor);

	//Next calculate potential
	//first calculate distance from plane
	float dist = abs(glm::dot(neighbor,growDirection));
	float pointDistance = dist;
	if (pointDistance > length)
		pointDistance = length;
	//find the radius at that distance
	float radius = (1-pointDistance/length)*bottomRadius;
	//now find the vector at the closest point to that circle
	//with that radius
	vec3 planeVector = pointDistance*growDirection;
	vec3 radiusSurfacePoint = planeVector+glm::normalize(neighbor-planeVector)*radius;
	//This distance is the factor used
	ratedNeighbors.push_back(pair<comparableVec3,float>(neighbor,glm::distance(radiusSurfacePoint,neighbor)));
}

void Building::placeVoxel(vec3 toVoxel) {


	//Now add neighbors
	for (float x = -1; x <= 1; x++) {
		for (float y = -1; y <= 1; y++) {
			for (float z = -1; z <= 1; z++) {
				vec3 pos(x,y,z);
				pos += toVoxel;
				if ((placed.find(pos) == placed.end()) &&
					(neighbors.find(pos) == neighbors.end()))
					addNeighbor(pos);
			}
		}
	}
}
*/

