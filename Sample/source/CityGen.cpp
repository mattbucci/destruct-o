#include "stdafx.h"
#include "CityGen.h"
#include "structure.h"

//the size of each city
const int citysize = 200;

//the space between each city
const int cityspacing = 400;

int CityGen::BufferIndex(GameTile* tile,int x, int y) {
	return 4 * (y*tile->Width + x);
}

void CityGen::construct_road(GameTile* tile, vec3 start, vec3 end){


}
void CityGen::construct_building(GameTile* tile, vec3 pos){
	//BEGIN BUILDING DEMO
	//Load building floors (note lack of extension)
	Structure * bottomFloor = Structure::LoadStructure("teststructures/testfloor30x10");
	Structure * transitionFloor = Structure::LoadStructure("teststructures/testfloor30x10to20x10");
	Structure * topFloor = Structure::LoadStructure("teststructures/testfloor20x10");

	//Randomize the number of floors a bit
	//Obviously more varied randomization would have to be done
	//for more variation in buildings
	int numberOfBaseFloors = rand() % 3;
	int numberOfTopFloors = rand() % 3;

	//A Building is used to construct a structure by stacking other structures (floors) together
	Building newBuilding;

	//Stack bottom floors
	for (int i = 0; i < numberOfTopFloors; i++)
		newBuilding.Stack(bottomFloor);

	//Add an adapter floor
	newBuilding.Stack(transitionFloor);

	//Add the rest
	for (int i = 0; i < numberOfTopFloors; i++)
		newBuilding.Stack(topFloor);

	//At this point some sort of roof should have been added
	//but I didn't make one

	//When you're all done stacking floors
	//call construct once to internally prepare the structure
	//for use
	newBuilding.ConstructStructure();

	//Then call PlaceStructure one or more times to place the structure
	//in whatever 2d positions you want
	//automatically compensates for terrain (no building voxels underneath the ground)
	newBuilding.PlaceStructure(tile,vec2(pos.x,pos.y));

}

void CityGen::construct_city(GameTile * tile, vec3 pos) {
	//flatten terrain
	//generate roads
	TileCell * cells = tile->Cells;
	for (int y = -citysize / 2; y < citysize / 2; y++) {
		for (int x = -citysize / 2; x < citysize / 2; x++) {
			cells[int((pos.y + y)*tile->Width + pos.x + x)].materialId = 3;
			if ((x + citysize / 2) % 50 == 0 && (y + citysize / 2) % 50 == 0) {
				construct_building(tile, vec3(pos.x + x, pos.y + y, pos.z));
			}
		}
	}
}

void CityGen::generatecitylocations(GameTile* tile){

	//the amount to blur
	const int blur = 10;

	//analyze the terrain
	//We Differentiate the terrain and then blur it 4x to achieve the effect of inteligent city placement

	//this will store the differential blur of the terrain
	float * analysis = new float[tile->Width*tile->Height];

	//This allows us to access the terrain values
	TileCell * cells = tile->Cells;

	//our operator ignores edges, so set them to be really high
	//This also helps keep cities off the edge of tiles because that breaks things
	for (int y = 0; y < tile->Height; y++) {
		//left
		analysis[y*tile->Height + 0] = 500;
		//right
		analysis[y*tile->Height + tile->Width - 1] = 500;
	}
	//check the x edge and normalize the height so it matches
	for (int x = 0; x < tile->Height; x++) {
		//top
		analysis[0 * tile->Height + x] = 500;
		//bottom
		analysis[(tile->Height - 1)*tile->Height + x] = 500;
	}

	//Get the slope using the Sobel Operator
	for (int y = 1; y < tile->Height - 1; y++) {
		for (int x = 1; x < tile->Width - 1; x++) {
			//directions don't actually matter, we just care about magnitude of change
			float bottomLeftIntensity = cells[tile->Width*(y - 1) + x - 1].height;
			float topRightIntensity = cells[tile->Width*(y + 1) + x + 1].height;
			float topLeftIntensity = cells[tile->Width*(y + 1) + x - 1].height;
			float bottomRightIntensity = cells[tile->Width*(y - 1) + x + 1].height;
			float leftIntensity = cells[tile->Width*(y)+x - 1].height;
			float rightIntensity = cells[tile->Width*(y)+x + 1].height;
			float bottomIntensity = cells[tile->Width*(y - 1) + x].height;
			float topIntensity = cells[tile->Width*(y + 1) + x].height;

			float h = -topLeftIntensity - 2.0 * topIntensity - topRightIntensity + bottomLeftIntensity + 2.0 * bottomIntensity + bottomRightIntensity;
			float v = -bottomLeftIntensity - 2.0 * leftIntensity - topLeftIntensity + bottomRightIntensity + 2.0 * rightIntensity + topRightIntensity;

			float mag = glm::length(vec2(h, v));
			analysis[y*tile->Height + x] = mag;
		}
	}

	//Apply Blur 4x
	for (int i = 0; i < blur; i++) {
		for (int y = 1; y < tile->Height - 1; y++) {
			for (int x = 1; x < tile->Width - 1; x++) {
				//directions don't actually matter, we just care about magnitude of change

				float bottomLeftIntensity = analysis[tile->Width*(y - 1) + x - 1];
				float topRightIntensity = analysis[tile->Width*(y + 1) + x + 1];
				float topLeftIntensity = analysis[tile->Width*(y + 1) + x - 1];
				float bottomRightIntensity = analysis[tile->Width*(y - 1) + x + 1];
				float leftIntensity = analysis[tile->Width*(y)+x - 1];
				float rightIntensity = analysis[tile->Width*(y)+x + 1];
				float bottomIntensity = analysis[tile->Width*(y - 1) + x];
				float topIntensity = analysis[tile->Width*(y + 1) + x];

				double top = topIntensity + topLeftIntensity + topRightIntensity;
				double middle = analysis[y*tile->Height + x] + leftIntensity + rightIntensity;
				double bottom = bottomIntensity + bottomLeftIntensity + bottomRightIntensity;

				float average = (top + middle + bottom) / 9;

				analysis[y*tile->Height + x] = average;
			}
		}
	}
	float minheight = 5000;
	vec3 citylocation;

	float noisetolerance = 1;
	float heighttolerance = 20;
	//figure out where to put the city but make sure it doesn't go off the edge
	for (int y = citysize/2; y < tile->Height - citysize/2; y++) {
		for (int x = citysize/2; x < tile->Width - citysize/2; x++) {
			if (analysis[y*tile->Height + x] < noisetolerance) {

				//check if we're too close to another city
				bool tooclose = false;
				for (int i = 0; i < tile->Cities.size(); i++) {
					if (glm::length(vec2(tile->Cities[i].x - x, tile->Cities[i].y - y)) < cityspacing)
						tooclose = true;
				}

				if (!tooclose) {
					if (abs(cells[y*tile->Height + x - citysize / 2].height - cells[y*tile->Height + x + citysize / 2].height) < heighttolerance &&
						abs(cells[(y - citysize / 2)*tile->Height + x].height - cells[(y + citysize / 2)*tile->Height + x].height) < heighttolerance
						) {
						//make some cities
						tile->Cities.push_back(vec3(x, y, 0));
						cout << "CITY PLACED AT:" << x << " , " << y << endl;
					}
				}
			}
		}
	}

}
void CityGen::GenerateCities(GameTile * tile) {
	generatecitylocations(tile);
	for (int i = 0; i < tile->Cities.size(); i++) {
		construct_city(tile, tile->Cities[i]);
	}
}