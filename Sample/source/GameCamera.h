
#pragma once

#include "stdafx.h"

#include "GL3DProgram.h"


class GameCamera {

	vec3 cameraPosition;
	float angleOfAttack;
	float rotation;

	//Matrices needed for unproject
	//taken from the last frame
	mat4 lastProjectionMatrix;
	mat4 lastViewMatrix;
	mat4 lastModelMatrix;
	vec2 viewPortSize;
	//camera control enabled by default
	bool cameraControlEnabled;
public:
	GameCamera();

	//Perform any camera movements
	void Update(double drawDelta,set<int> currentlyPressedKeys);

	//Should be called before any unproject calls
	void UpdateViewSize(vec2 viewSize);

	void Draw(GL3DProgram * shaders);

	void Rotation(float rotation);
	void Position(vec3 pos);
	vec3 Position();
	void AOA(float angle);

	//Determine the min/max point of visible extents given the min/max height
	//Combines Unproject with UnprojectToGround
	//Not implemented currently
	pair<vec2,vec2> FindExtents(float minZ, float maxZ);
	//Given a screen coordinate, what 3d vector does that coordinate represent
	//in game
	//The return value is location,direction
	pair<vec3,vec3> Unproject(vec2 pos);
	//Given a screen coordinate find the location on the terrain that matches that click
	//That is, where on the ground did the user click
	//doesn't take into account the varying nature of the terrain yet
	vec2 UnprojectToGround(vec2 pos, float groundHeight = 0.0);

};