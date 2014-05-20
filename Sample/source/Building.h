
#pragma once

#include "stdafx.h"
#include "Structure.h"

class GameTile;



class comparableVec3 {
public:
	comparableVec3() {
		x=y=z=0;
	}
	comparableVec3(vec3 original) {
		x = original.x;
		y = original.y;
		z = original.z;
	}
	comparableVec3(const comparableVec3 & b) {
		x = b.x;
		y = b.y;
		z = b.z;
	}

	comparableVec3 & operator=(const comparableVec3 & b) {
		x = b.x;
		y = b.y;
		z = b.z;
		return *this;
	}

	bool operator<(const comparableVec3 & b) const {
		if (x < b.x)
			return true;
		if (x > b.x)
			return false;

		if (y < b.y)
			return true;
		if (y > b.y)
			return false;

		return z < b.z;
	}

	bool operator==(const comparableVec3 & b) const {
		return (x == b.x) && (y == b.y) && (z == b.z);
	}

	float x;
	float y;
	float z;
};

//bool operator<(const glm::vec3 & a, const glm::vec3 & b);


class Building {
	Structure constructed;

	ContiguousList<Structure*> floors;

	//Spire grow data
	set<comparableVec3> placed;
	vec3 growDirection;
	float bottomRadius;
	float length;

	void addNeighbor(vec3 neighbor);
	void placeVoxel(vec3 toVoxel);

public:
	Building();

	//Add a structure to the stack 
	//note: the structure pointer *must* stay valid
	//until the last call to ConstructStructure()
	void Stack(Structure * floor);

	//Construct a structure out of every floor given
	//only needs to be called once
	void ConstructStructure();

	//If instead you wish to build a spire instead of a stack call this
	void ConstructSpire(vec3 growDirection, float bottomRadius, float length);

	//Place a structure at a given position on the tile
	//automatically adds the structure at that position
	//compensating for terrain automagically
	void PlaceStructure(GameTile * onTile, vec2 position);
};