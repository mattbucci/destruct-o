
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

	vec3 position;
	vec3 velocity;
	//Spawn time
	double spawn;
	ParticleSystem * owner;

	//Draw this single particle. Must be called form the global context
	void Draw(const mat4 & view, GL3DProgram * shaders);
	//Updates the position of the particle
	bool Update(double time, double delta);
};
