#pragma once

#include "LinearInterop.h"
#include "InteropChain.h"

#define LinearChain(x) InteropChain<LinearInterop<x>,x>

class ParticleData {
public:
	//This will load from a file eventually

	//These properties use the emitter's life as the time
	//if they effect particles they are decided on particle creation
	//The rate which particles are generated
	LinearChain(float) GenerationRate;
	//The initial velocity vector
	LinearChain(vec3) Velocity;
	//The variation in velocity of generated particles (%)
	LinearChain(float) Variation;
	//X,Y is min, max of random latitude
	//applied to starting velocity
	LinearChain(vec2) Latitude;
	//X,Y is min, max of random longitude
	//applied to starting velocity
	LinearChain(vec2) Longitude;
	//The life of the emitted particle
	LinearChain(float) Life;
	//The width/length of emitter
	LinearChain(vec2) EmitterSize;
	//The rows/columns of the particle texture
	//note, floor() is applied before rows/columns are used
	float Rows;
	float Columns;
	//The first frame of the texture the particle uses
	//the first value is the min
	//the second value is the max, the used value is floor(random(min,max))
	LinearChain(vec2) FrameOffset;

	//These properties use the particle's life as the time
	//The color of the generated particle
	LinearChain(vec4) Color;
	//The scale of the generated particle
	LinearChain(vec3) Scale;
	//The acceleration applied to the particle
	LinearChain(vec3) Acceleration;
	//The speed at which to play frames from the texture
	//in frames per second
	LinearChain(float) AnimationSpeed;


	enum MaterialEffect {
		NONE,
		BLEND,
		ADDITIVE,
		//Similar to additive, but reverses color first
		SCREEN,
	};


	MaterialEffect Particle;
};