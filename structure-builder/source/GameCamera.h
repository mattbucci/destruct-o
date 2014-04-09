
#pragma once

#include "stdafx.h"

#include "GL3DProgram.h"


class GameCamera {
	vec3 position;

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

	//Move the camera focus point
	void SetCameraView(vec3 focusPoint);
	//get camera position
	vec3 GetPosition();
	//get camera direction
	vec3 GetDirection();

	
	float Pan;
	float Tilt;

	float Distance;

	//Given a screen coordinate, what 3d vector does that coordinate represent
	//in game
	//The return value is location,direction
	pair<vec3,vec3> Unproject(vec2 pos);
	//Given a screen coordinate find the location on the terrain that matches that click
	//That is, where on the ground did the user click
	//doesn't take into account the varying nature of the terrain yet
	vec2 UnprojectToGround(vec2 pos, float groundHeight = 0.0);

	//A helper function. Given a ray, calculate it's distance from the given plane
	static double CalculateIntersectionDistance(vec3 rayStart, vec3 rayDirection, vec3 planePoint, vec3 planeNormal);
};