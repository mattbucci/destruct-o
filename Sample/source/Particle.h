
#pragma once
#include "GL3DProgram.h"


class ParticleSystem;
class ParticleData;

class Particle {
	double unroundedFrame;
	//Saved scale variation determined at spawn
	float scaleVariation;
	//How long this particle will live in total
	float lifeSpan;
public:
	//"systemTime" is the time relative to the spawning particle system
	//that is, if the game has been running for 100 seconds and the particle system has been alive for 10
	//game time would be 100, system time would be 10
	Particle(double gameTime, float systemTime, ParticleSystem * owner, ParticleData * systemData);
	//Data calculated by last Update
	vec3 Position;
	int Frame;
	float Scale;
	vec3 Velocity;
	vec4 Color;

	//Spawn Information
	double spawnedAt;
	double deathAt;
	ParticleSystem * Owner;
	ParticleData * SystemData;

	//Updates the position of the particle
	//returns true if the particle life has expired
	bool Update(double time, double delta);
};
 