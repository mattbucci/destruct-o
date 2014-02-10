
#pragma once

#include "stdafx.h"

#include "GL3DProgram.h"


class GameCamera {
	vec3 position;
	vec3 direction;

	//Matrices needed for unproject
	//taken from the last frame
	mat4 lastProjectionMatrix;
	mat4 lastViewMatrix;
	mat4 lastModelMatrix;
	vec2 viewPortSize;

public:
	GameCamera();

	//Should be called before any unproject calls
	void UpdateViewSize(vec2 viewSize);

	//Apply the camera and global lighting
	void Draw(GL3DProgram * shaders);

	//Move the camera view point
	void SetCameraView(vec3 position, vec3 direction);
	//get camera position
	vec3 GetPosition();
	//get camera direction
	vec3 GetDirection();


	//Given a screen coordinate, what 3d vector does that coordinate represent
	//in game
	//The return value is location,direction
	pair<vec3,vec3> Unproject(vec2 pos);
	//Given a screen coordinate find the location on the terrain that matches that click
	//That is, where on the ground did the user click
	//doesn't take into account the varying nature of the terrain yet
	vec2 UnprojectToGround(vec2 pos, float groundHeight = 0.0);

};