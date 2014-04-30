
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

	float winduptime;
	float winddowntime;
	bool autoStateChange;

public:
	EffectLaser(vec4 laserColor = vec4(1,1,1,1), float laserWidth=.1f);
	~EffectLaser();

	//Set windup/winddown time
	//setting autostatechange means a single call to StartFiring()
	//will cause the laser to run through firingup/coolingdown
	void SetTiming(float winduptime,float winddowntime, bool autoStateChange);

	//Fire-up the laser
	void StartFiring();

	//Spin down the laser
	void StopFiring();


	//Must call move after changing these properties
	void SetLaserColor(vec4 laserColor);
	void SetLaserWidth(float laserWidth);


	//Set firing position
	void Move(vec3 startingPos, vec3 endPoint);

	//Update the laser, returns true if the laser is above 0 power and should be drawn
	bool Update();

	//Draw only necessary when Update() returns true
	//to see laser call StartFiring()
	void Draw(GLEffectProgram * program);
};