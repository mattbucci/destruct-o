
#pragma once

#include "stdafx.h"

#include "Savable.h"

class SavableCityData : public Savable {
public:
	vec3 cityPosition;
	//The tallest voxel in the tower
	vec2 cityCenterVoxel;
	//When this is flattened city changes owners
	//this is the original height of the voxel
	float originalCityCenterVoxel;
	//Whether or not the city is owned by the player
	bool ownedByPlayer;
	//gun turret positions
	vector<vec3> gunPositions;
	//whether or not each turret is alive
	vector<bool> gunAlive;

	CLASS_DECLARATION(SavableCityData)
		CLASS_MEMBER(cityPosition,ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(cityCenterVoxel,ReflectionData::SAVE_VEC2)
		CLASS_MEMBER(originalCityCenterVoxel,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(ownedByPlayer,ReflectionData::SAVE_BOOL)
		CLASS_CONTAINER_MEMBER(gunPositions,ReflectionData::SAVE_VECTOR,ReflectionData::SAVE_VEC3)
		CLASS_CONTAINER_MEMBER(gunAlive,ReflectionData::SAVE_VECTOR,ReflectionData::SAVE_BOOL)
	END_DECLARATION
};