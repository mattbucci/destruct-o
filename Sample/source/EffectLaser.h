
#pragma once

#include "stdafx.h"
#include "GLEffectVertexGroup.h"

class GLEffectProgram;

class EffectLaser : GLEffectVertexGroup {
	vec3 startingPos;
	vec3 direction;

	enum LaserState {
		LASER_IDLE,
		LASER_WINDINGUP,
		LASER_FIRING,
		LASER_WINDINGDOWN,
	};
	//current state
	LaserState state;
	//When you started this state
	double lastUpdated;

	//The color of this laser
	vec4 laserColor;

	//The width of the laser
	float laserWidth;

	//How much "power" is in the laser 1 means 100%
	float laserPower;
public:
	EffectLaser(vec4 laserColor, float laserWidth=.1f);
	~EffectLaser();

	//Fire-up the laser
	void StartFiring();

	//Spin down the laser
	void StopFiring();

	//Set firing position
	void Move(vec3 startingPos, vec3 endPoint);

	//Draw
	//call always
	//to see laser call StartFiring()
	void Draw(GLEffectProgram * program);
};