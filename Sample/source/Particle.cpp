#include "stdafx.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "OS.h"
#include "ParticleData.h"
#include "Utilities.h"


Particle::Particle(double gameTime, float systemLifeFactor, ParticleSystem * owner, ParticleData * systemData) {
	this->Owner = owner;
	this->SystemData = systemData;

	//Determine all your starting properties
	
	//Calculate velocity factoring in variation
	float variationPercent = systemData->VelocityVariation.ValueAtSequence(systemLifeFactor);
	this->Velocity = systemData->Velocity.ValueAtSequence(systemLifeFactor) * 
		(1.0f + variationPercent - Utilities::random(0.0f,2.0f*variationPercent));

	//Apply latitude/longitude to initial velocity vector
	vec4 tempVelocity = vec4(this->Velocity,1.0);
	vec2 latitudeRange = systemData->Latitude.ValueAtSequence(systemLifeFactor);
	vec2 longitudeRange = systemData->Longitude.ValueAtSequence(systemLifeFactor);
	//First apply latitude
	tempVelocity = tempVelocity * glm::rotate(Utilities::random(latitudeRange.x,latitudeRange.y),vec3(1,0,0));
	//Now longitude
	tempVelocity = tempVelocity * glm::rotate(Utilities::random(longitudeRange.x,longitudeRange.y),vec3(0,0,1));
	this->Velocity = vec3(tempVelocity);

	//Determine initial position
	vec2 emitterSize = systemData->EmitterSize.ValueAtSequence(systemLifeFactor);
	Position.x = emitterSize.x * .5f - Utilities::random(0.0f,emitterSize.x);
	Position.y = emitterSize.y * .5f - Utilities::random(0.0f,emitterSize.y);
	Position += owner->Position;

	//Apply your initial frame
	vec2 frameRange = SystemData->FrameOffset.ValueAtSequence(systemLifeFactor);
	Frame = (int)(Utilities::random(frameRange.x,frameRange.y));
	unroundedFrame = (double)Frame;

	//Save the scale variation that applies to you
	float possibleVariation = systemData->ScaleVariation.ValueAtSequence(systemLifeFactor);
	scaleVariation = 1+Utilities::random(-possibleVariation,possibleVariation);;

	//Set your spawn time to right now
	spawnedAt = gameTime;
	vec2 lifeRange = systemData->Life.ValueAtSequence(systemLifeFactor);
	lifeSpan = Utilities::random(lifeRange.x,lifeRange.y);
	deathAt = spawnedAt+lifeSpan;
}

//Updates the position of the particle
bool Particle::Update(double time, double delta) {
	if (time >= deathAt)
		//Mark this particle for destruction
		return true;

	//Simulate particle
	float life = (float)(time-spawnedAt);
	//How long this particle has lived normalized to be from 0.0 to 1.0
	float lifeFactor = 1.0f - (life/lifeSpan);
	Color = SystemData->Color.ValueAtSequence(lifeFactor);
	Scale = SystemData->Scale.ValueAtSequence(lifeFactor) * scaleVariation;
	//Now get/apply forces
	vec3 acceleration = SystemData->Acceleration.ValueAtSequence(lifeFactor);
	Velocity += acceleration*(float)delta;
	Position += Velocity*(float)delta;
	//Finally calculate the current frame
	unroundedFrame += SystemData->AnimationSpeed.ValueAtSequence(lifeFactor)*delta;
	Frame = (int)unroundedFrame;

	return false;
}