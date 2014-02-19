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
	//Example: 10 would be 10%, and any Velocity value could be up to 10% higher or lower
	LinearChain(float) Variation;
	//X,Y is min, max of random latitude (degrees -90 to 90)
	//applied to starting velocity
	LinearChain(vec2) Latitude;
	//X,Y is min, max of random longitude (degrees -180 to 180)
	//applied to starting velocity
	LinearChain(vec2) Longitude;
	//X,Y is the min/max of the life of the emitted particle
	LinearChain(vec2) Life;
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
	//X,Y is the min/max scale
	LinearChain(vec2) Scale;
	//The acceleration applied to the particle
	//this acceleration is /not/ rotated 
	LinearChain(vec3) Acceleration;
	//The speed at which to play frames from the texture
	//in frames per second
	LinearChain(float) AnimationSpeed;

	//Properties for the material
	enum MaterialEffect {
		//No blending (like voxels)
		NONE,
		//Transparency is allowed
		//sorting is required
		BLEND,
		//Color is added to already drawn colors
		ADDITIVE,
		//Similar to additive, but reverses color first
		SCREEN,
	};
	MaterialEffect MaterialStyle;
	//Should be the texture name relative to the particle texture directory
	string MaterialTexture;
}; 