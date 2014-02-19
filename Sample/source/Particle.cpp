#include "stdafx.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "OS.h"
#include "ParticleData.h"

//Note this should be replaced probably
template <class T>
static T random(T min, T max) {
	return ((float)rand()/(float)(RAND_MAX))*(max-min)+min;
}

Particle::Particle(double gameTime, float systemTime, ParticleSystem * owner, ParticleData * systemData) {
	this->Owner = owner;
	this->SystemData = systemData;

	//Determine all your starting properties
	
	//Calculate velocity factoring in variation
	float variationPercent = systemData->Variation.ValueAtSequence(systemTime);
	this->Velocity = systemData->Velocity.ValueAtSequence(systemTime) * 
		(1.0f + variationPercent - random(0.0f,2.0f*variationPercent));

	//Apply the latitude/longitude to velocity
	vec2 latRange = systemData->Latitude.ValueAtSequence(systemTime);
	float lat = random(latRange.x,latRange.y);
	vec2 lonRange = systemData->Longitude.ValueAtSequence(systemTime);
	float lon = random(lonRange.x,lonRange.y);
	//TODO: Finsh lat/lon

	//Determine initial position
	vec2 emitterSize = systemData->EmitterSize.ValueAtSequence(systemTime);
	Position.x = emitterSize.x * .5f - random(0.0f,emitterSize.x);
	Position.y = emitterSize.y * .5f - random(0.0f,emitterSize.y);
	Position += owner->Position;

	//Apply your initial frame
	vec2 frameRange = SystemData->FrameOffset.ValueAtSequence(systemTime);
	Frame = (int)(random(frameRange.x,frameRange.y));
	unroundedFrame = (double)Frame;

	//Set your spawn time to right now
	spawnedAt = gameTime;
	vec2 lifeRange = systemData->Life.ValueAtSequence(systemTime);
	deathAt = spawnedAt+random(lifeRange.x,lifeRange.y);
}

//Updates the position of the particle
bool Particle::Update(double time, double delta) {
	if (time >= deathAt)
		//Mark this particle for destruction
		return true;

	//Simulate particle
	float life = (float)(time-spawnedAt);
	Color = SystemData->Color.ValueAtSequence(life);
	vec2 scaleRange = SystemData->Scale.ValueAtSequence(life);
	Scale = random(scaleRange.x,scaleRange.y);
	//Now get/apply forces
	vec3 acceleration = SystemData->Acceleration.ValueAtSequence(life);
	Velocity += acceleration*(float)delta;
	Position += Velocity*(float)delta;
	//Finally calculate the current frame
	unroundedFrame += SystemData->AnimationSpeed.ValueAtSequence(life)*delta;
	Frame = (int)unroundedFrame;

	return false;
}