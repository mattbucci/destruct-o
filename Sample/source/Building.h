
#pragma once

#include "stdafx.h"
#include "Structure.h"

class GameTile;

class Building {
	Structure constructed;

	ContiguousList<Structure*> floors;
public:
	Building();

	//Add a structure to the stack 
	//note: the structure pointer *must* stay valid
	//until the last call to ConstructStructure()
	void Stack(Structure * floor);

	//Construct a structure out of every floor given
	//only needs to be called once
	void ConstructStructure();

	//Place a structure at a given position on the tile
	//automatically adds the structure at that position
	//compensating for terrain automagically
	void PlaceStructure(GameTile * onTile, vec2 position);
};