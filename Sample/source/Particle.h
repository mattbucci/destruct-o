
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
	//systemLifeFactor is a a value 0 - 1 which represents how long the system (not the particle) has been
	//alive for, 0 is just started, and 1 being the end point of the system
	//if the particle system has infinite life, this will always be 1
	Particle(double gameTime, float systemLifeFactor, ParticleSystem * owner, ParticleData * systemData);
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
 