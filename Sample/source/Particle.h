
#pragma once
#include "GL3DProgram.h"
#include "GLDynamicVertexGroup.h"

class GL3DProgram;


class ParticleSystem;
class ParticleData;

class Particle : GLDynamicVertexGroup {
	//Generated properties
	//Inherited properties
		
	ParticleData * systemData;
	//The particles predicted end 
	double deathAt;
	//properties calculate during last update
	vec4 color;
	vec3 scale;
public:
	Particle(double gameTime, float systemTime, ParticleSystem * owner, ParticleData * systemData);
	//Data calculated by last Update
	vec3 Position;
	int Frame;
	float Scale;
	vec3 Velocity;
	//Spawn time
	double spawn;
	ParticleSystem * owner;
	ParticleData * SystemData;

	//Draw this single particle. Must be called form the global context
	void Draw(const mat4 & view, GL3DProgram * shaders);
	//Updates the position of the particle
	bool Update(double time, double delta);
};
