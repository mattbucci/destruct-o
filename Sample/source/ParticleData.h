#pragma once

#include "LinearInterop.h"
#include "InteropChain.h"

#define LinearChain(x) InteropChain<LinearInterop<x>,x>

class ParticleData {
public:
	//This will load from a file eventaully

	//Length before particle system ends
	//negative for forever
	double SystemLifetime;

	//These properties use the emitter's life as the time
	//if they effect particles they are decided on particle creation
	//The rate which particles are generated
	LinearChain(float) GenerationRate;
	//The initial velocity vector
	LinearChain(vec3) Velocity;
	//The variation in velocity of generated particles (%)
	LinearChain(float) Variation;
	//The life of the emitted particle
	LinearChain(float) Life;
	//The width/length of emitter
	LinearChain(vec2) EmitterSize;

	//These properties use the particle's life as the time
	//The color of generated particles
	LinearChain(vec4) Color;
	//The scale of generated particles
	LinearChain(vec3) Scale;
	//The acceleration applied to particles
	LinearChain(vec3) Acceleration;


	//A function used to create interesting particle movement
	//is called, should return acceleration (added to the above Acceleration)
	//parameters: life remaining (scaled 1 to 0), position, velocity
	//should return acceleration
	function<vec3(float,vec3,vec3)> ForceFunction;

	enum MaterialEffect {
		NONE,
		BLEND,
		ADDITIVE,
		MODULATE,
	};


	//MaterialEffect Particle;
};