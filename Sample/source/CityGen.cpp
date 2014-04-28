#include "stdafx.h"
#include "CityGen.h"
#include "Structure.h"
#include "Rect.h"
#include "Utilities.h"
#include "SavableCityData.h"

#include "BaseFrame.h"
#include "ActorSystem.h"
#include "ActorAids.h"
//the size of each city
const int citysize = 150;

//the space between each city
const int cityspacing = 250;



int CityGen::BufferIndex(GameTile* tile,int x, int y) {
	return 4 * (y*TILE_SIZE + x);
}

void CityGen::construct_road(GameTile* tile, vec3 start, vec3 end){


}
void CityGen::construct_building(GameTile* tile, vec3 pos)
{
	//BEGIN BUILDING DEMO
	//Load building floors (note lack of extension)
	Structure * bottomFloor = Structure::LoadStructure("teststructures/testfloor30x10");
	Structure * transitionFloor = Structure::LoadStructure("teststructures/testfloor30x10to20x10");
	Structure * topFloor = Structure::LoadStructure("teststructures/testfloor20x10");

	//Randomize the number of floors a bit
	//Obviously more varied randomization would have to be done
	//for more variation in buildings
	//int numberOfBaseFloors = rand() % 3;
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

void CityGen::construct_city(GameTile * tile, vec3 pos)
{
	ContiguousList<Rect> buildingPlacements;
	
	TileCell * cells = tile->Cells;

	vec3 tileOffset = vec3(tile->tile_x,tile->tile_y,0) * vec3(TILE_SIZE,TILE_SIZE,1.0);

	//Warning HACK
	//create savable city data here
	SavableCityData * cityData = new SavableCityData();
	cityData->cityPosition = pos+tileOffset;
	cityData->ownedByPlayer = false;

	float originalCityHeight = cells[int((pos.y)*TILE_SIZE + pos.x)].height;

	//Generate lines
	//set<vec2> usedSpace;
	static const int lineCount = 30;
	static const int turretSkip = 6;
	static const int samples = 200;
	static const float anglePart = (float)(M_PI*2.0f/lineCount);
    
    //flatten terrain
	//generate roads
	
	for (int y = -citysize / 2; y < citysize / 2; y++) {
		for (int x = -citysize / 2; x < citysize / 2; x++) {
			cells[int((pos.y + y)*TILE_SIZE + pos.x + x)].materialId = 2;
		}
	}
    
	for (int p = 0; p < lineCount; p++) {
		vec2 lineDirection = glm::normalize(vec2(cos(anglePart*p),sin(anglePart*p)));
		float lineLength = citysize*1.5;
		float lineSection = lineLength/samples;


		if ((p % turretSkip) == 0) {
			//Place turret
			vec2 spos = glm::floor(lineDirection*(-lineLength/2.0f+samples/4*lineSection)) + vec2(pos);

			//raise the height around the turrent
			float newHeight = originalCityHeight +10.0f;
			for (int x = -1; x <= 0; x++) {
				for (int y = -1; y <= 0; y++) {
					vec2 absPos = spos+vec2(x,y);
					TileCell & cell = cells[int((absPos.y)*TILE_SIZE + absPos.x)];
					cell.height = (unsigned char)newHeight;
				}
			}


			cityData->gunAlive.push_back(true);
			cityData->gunPositions.push_back(tileOffset+vec3(spos,newHeight));
		}

		//Generate the line by sampling along it
		
		for (int i = 0; i < samples; i++) {
			//Find the position
			vec2 spos = lineDirection*(-lineLength/2.0f+i*lineSection);
			spos = glm::floor(spos);
			//check that the position is within city limits
			if ((spos.x < -citysize/2.0) || (spos.y < -citysize/2.0))
				continue;
			if ((spos.x > citysize/2.0) || (spos.y > citysize/2.0))
				continue;
			//check that the position isn't too close to the center
			float len = glm::length(spos);
			if (len < 16)
				continue;
			//Place a line here
			float height = (float)((len*len-15*15)*.0008+2);
			vec2 absPos = glm::floor(vec2(pos)+spos);
			TileCell & cell = cells[int((absPos.y)*TILE_SIZE + absPos.x)];
			cell.height += (unsigned char)height;
			//City is stronger than surrounding terrain
			cell.cellHealth = 25;
			cell.cellMaxHealth = 25;
		}
	}

	//Different heights in the spire
	int ringHeights[5] = {
		30,
		28,
		24,
		18,
		10
	};


	//Place the spire
	for (float x = -4; x <= 4; x++) {
		for (float y = -4; y <= 4; y++) {
			float ring = max(abs(x),abs(y));
			vec2 spos = vec2(x,y)+vec2(pos);
			TileCell & cell = cells[int((spos.y)*TILE_SIZE + spos.x)];
			cell.height += ringHeights[(int)ring]*3/2;
			//The tower is made of tough stuff
			cell.cellHealth = 50;
			cell.cellMaxHealth = 50;
            cell.materialId = 3;
			//Register the center
			if (ring == 0) {
				cityData->cityCenterVoxel = spos+vec2(tileOffset);
				cityData->originalCityCenterVoxel = cell.height;
			}
				
		} 
	}

	//Super hack, transfer city data to ai
	Game()->Actors.Aids()->RegisterNewCity(cityData);

}

void CityGen::generatecitylocations(GameTile* tile){

	//the amount to blur
	const int blur = 4;

	//analyze the terrain
	//We Differentiate the terrain and then blur it 4x to achieve the effect of inteligent city placement

	//this will store the differential blur of the terrain
	float * analysis = new float[TILE_SIZE*TILE_SIZE];

	//This allows us to access the terrain values
	TileCell * cells = tile->Cells;

	//our operator ignores edges, so set them to be really high
	//This also helps keep cities off the edge of tiles because that breaks things
	for (int y = 0; y < TILE_SIZE; y++) {
		//left
		analysis[y*TILE_SIZE + 0] = 500;
		//right
		analysis[y*TILE_SIZE + TILE_SIZE - 1] = 500;
	}
	//check the x edge and normalize the height so it matches
	for (int x = 0; x < TILE_SIZE; x++) {
		//top
		analysis[0 * TILE_SIZE + x] = 500;
		//bottom
		analysis[(TILE_SIZE - 1)*TILE_SIZE + x] = 500;
	}

	//Get the slope using the Sobel Operator
	for (int y = 1; y < TILE_SIZE - 1; y++) {
		for (int x = 1; x < TILE_SIZE - 1; x++) {
			//directions don't actually matter, we just care about magnitude of change
			float bottomLeftIntensity = cells[TILE_SIZE*(y - 1) + x - 1].height;
			float topRightIntensity = cells[TILE_SIZE*(y + 1) + x + 1].height;
			float topLeftIntensity = cells[TILE_SIZE*(y + 1) + x - 1].height;
			float bottomRightIntensity = cells[TILE_SIZE*(y - 1) + x + 1].height;
			float leftIntensity = cells[TILE_SIZE*(y)+x - 1].height;
			float rightIntensity = cells[TILE_SIZE*(y)+x + 1].height;
			float bottomIntensity = cells[TILE_SIZE*(y - 1) + x].height;
			float topIntensity = cells[TILE_SIZE*(y + 1) + x].height;

			float h = -topLeftIntensity - 2.0f * topIntensity - topRightIntensity + bottomLeftIntensity + 2.0f * bottomIntensity + bottomRightIntensity;
			float v = -bottomLeftIntensity - 2.0f * leftIntensity - topLeftIntensity + bottomRightIntensity + 2.0f * rightIntensity + topRightIntensity;

			float mag = glm::length(vec2(h, v));
			analysis[y*TILE_SIZE + x] = mag;
		}
	}

	//Apply Blur 4x
	for (int i = 0; i < blur; i++) {
		for (int y = 1; y < TILE_SIZE - 1; y++) {
			for (int x = 1; x < TILE_SIZE - 1; x++) {
				//directions don't actually matter, we just care about magnitude of change

				float bottomLeftIntensity = analysis[TILE_SIZE*(y - 1) + x - 1];
				float topRightIntensity = analysis[TILE_SIZE*(y + 1) + x + 1];
				float topLeftIntensity = analysis[TILE_SIZE*(y + 1) + x - 1];
				float bottomRightIntensity = analysis[TILE_SIZE*(y - 1) + x + 1];
				float leftIntensity = analysis[TILE_SIZE*(y)+x - 1];
				float rightIntensity = analysis[TILE_SIZE*(y)+x + 1];
				float bottomIntensity = analysis[TILE_SIZE*(y - 1) + x];
				float topIntensity = analysis[TILE_SIZE*(y + 1) + x];

				double top = topIntensity + topLeftIntensity + topRightIntensity;
				double middle = analysis[y*TILE_SIZE + x] + leftIntensity + rightIntensity;
				double bottom = bottomIntensity + bottomLeftIntensity + bottomRightIntensity;

				float average = (float)(top + middle + bottom) / 9;

				analysis[y*TILE_SIZE + x] = average;
			}
		}
	}

	vec3 citylocation;

	float noisetolerance = 3;
	float heighttolerance = 5;
	//figure out where to put the city but make sure it doesn't go off the edge
	for (int y = citysize/2+10; y < TILE_SIZE - (citysize/2+10); y++) {
		for (int x = citysize/2+10; x < TILE_SIZE - (citysize/2+10); x++) {
			if (analysis[y*TILE_SIZE + x] < noisetolerance) {

				//check if we're too close to another city
				bool tooclose = false;
				for (unsigned int i = 0; i < tile->Cities.size(); i++) {
					if (glm::length(vec2(tile->Cities[i].x - x, tile->Cities[i].y - y)) < cityspacing)
						tooclose = true;
				}

				if (!tooclose) {
					if (abs(cells[(y - citysize / 2)*TILE_SIZE + x - citysize / 2].height - cells[(y + citysize / 2)*TILE_SIZE + x - citysize / 2].height) < heighttolerance &&
						abs(cells[(y - citysize / 2)*TILE_SIZE + x + citysize / 2].height - cells[(y + citysize / 2)*TILE_SIZE + x + citysize / 2].height) < heighttolerance &&
						abs(cells[(y + citysize / 2)*TILE_SIZE + x + citysize / 2].height - cells[(y + citysize / 2)*TILE_SIZE + x - citysize / 2].height) < heighttolerance &&
						abs(cells[(y - citysize / 2)*TILE_SIZE + x + citysize / 2].height - cells[(y - citysize / 2)*TILE_SIZE + x - citysize / 2].height) < heighttolerance
						) {
						//make some cities
						tile->Cities.push_back(vec3(x, y, 0));
						cout << "CITY PLACED AT:" << x << " , " << y << endl;
					}
				}
			}
		}
	}

	//Flatten Terrain
	for (unsigned int i = 0; i < tile->Cities.size(); i++) {
		vec3 pos = tile->Cities[i];
		for (int y = -citysize / 2; y < citysize / 2; y++) {
			for (int x = -citysize / 2; x < citysize / 2; x++) {
				cells[int((pos.y + y)*TILE_SIZE + pos.x + x)].height = cells[int((pos.y)*TILE_SIZE + pos.x)].height;
			}
		}
	}

	// TODO : Blur city edges

	/*
	for (int i = 0; i < tile->Cities.size(); i++) {
		vec3 pos = tile->Cities[i];
		//walk along the edge, for each edge walk outward and normalize
		for (int y = pos.y - citysize/2; y < pos.y + citysize/2; y++) {
			//left
			analysis[y*TILE_SIZE + TILE_SIZE - 1] = 500;

			//right
			analysis[y*TILE_SIZE + TILE_SIZE - 1] = 500;
		}
		for (int x = pos.x - citysize / 2; x < pos.x + citysize / 2; x++) {
			//top
			analysis[0 * TILE_SIZE + x] = 500;
			//bottom
			analysis[(TILE_SIZE - 1)*TILE_SIZE + x] = 500;
		}
	}
	*/
	delete [] analysis;
}
void CityGen::GenerateCities(GameTile * tile) {
	generatecitylocations(tile);
	for (unsigned int i = 0; i < tile->Cities.size(); i++) {
		construct_city(tile, tile->Cities[i]);
	}
	tile->UpdateTileSection(0, 0, TILE_SIZE, TILE_SIZE);
}
