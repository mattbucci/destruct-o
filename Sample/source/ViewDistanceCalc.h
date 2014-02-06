#pragma once

#include "stdafx.h"

class GL3DProgram;

//Learning parameters are within the CalculateAndApply function
#define MAX_DRAW_DISTANCE 200
#define MIN_DRAW_DISTANCE 20

class ViewDistanceCalc {
	//Clamped/Scaled between 0.0 and 1.0
	//Where 0 is MIN_DRAW_DISTANCE
	//and 1 is MAX_DRAW_DISTANCE
	float viewDistance;
	int framesDrawn;
public:
	ViewDistanceCalc();

	//Should be called every draw cycle
	//calculates the view distance
	//applies to fog distance automatically
	void CalculateAndApply(GL3DProgram * shader,float currentFPS);

	//Retrieve a pair of coordinates representing the appropriate draw section
	//the pair of coordinates is min point and max point
	//they form the upper left and lower right corners of a rectangle of voxels to draw
	pair<vec2,vec2> VoxDrawCoordinates(vec2 viewPortSize, vec2 mapExtents, vec2 playerPosition, float playerFacing);

	//Get's the view distance in units
	//note the last voxel the user can actually see is about 80% of this
	float GetViewDistance();
};