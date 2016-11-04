/**
 * Copyright (c) 2016, Nathaniel R. Lewis, Anthony Popelar, Matt Bucci, Brian Bamsch
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __ACTOR_H__
#define __ACTOR_H__

#include "stdafx.h"

class ActorSystem;
class BaseFrame;
class ModelInstance;
class MaterialProgram;
class PhysicsActor;
class GLEffectProgram;

class Actor : public Savable {
	bool valid;

	//The last time an animation ended (or looped)
	double lastAnimationEndTime;

	//The name of the model being used
	string modelName;

	//The last animation being played
	string lastPlayedAnimation;

	//Actors now own position
	vec3 position;

	//This class' encapsulation is leaky
	//This could be fixed if we have more time
	//The physics system has direct access to position/velocity/acceleration
	friend class PhysicsSystem;
	//So does the physics actor
	friend class PhysicsActor;
	//So does the actor system
	friend class ActorSystem;
protected:
	//When an actor is loaded
	//handles rebuilding the model instance
	virtual void Load(Json::Value & parentValue, LoadData & loadData) override;

	//Safely erase this actor during the next update cycle
	void Destroy();

	//Play an animation
	//also allows animationRunning() to work
	void playAnimation(const string & animationName);

	//Starts playing the animation if the animation is not the same one
	//that is currently playing
	void setAnimation(const string & animationName);

	//Set the current model
	//if modelName is invalid
	//model will still be NULL
	void setModel(const string & modelName);

	//Checks if an animation is running
	//true if the animation is running
	//false if the animation has looped
	bool animationRunning();

	//Facing direction for the model mostly
	//in radians
	float facingDirection;

	//The tilt of the model along the facing direction
	//in radians (defaults to 0)
	float tiltDirection;

	//The scale applied to the model
	float scale;

	//Just a shortcut to the actors
	ActorSystem & Actors();

    //Direct access to the model is available if necessary
    ModelInstance * model;
public:
	//Automatically registers this actor
	Actor();
	virtual ~Actor();

	//Update this actor
	//returns true if the actor should be destroyed
	//otherwise return false
	virtual bool Update();

	//If this object is about to be erased
	//this returns true
	virtual bool Dead();

	//Anyone can know the position
	vec3 GetPosition();

	//Draw this actor
	//And any associated model
	virtual void Draw(MaterialProgram * materialShader);

	//Draw any associated effects
	virtual void Draw(GLEffectProgram * effectShader);

	CLASS_DECLARATION(Actor)
		CLASS_MEMBER(position,ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(valid,ReflectionData::SAVE_BOOL)
		CLASS_MEMBER(facingDirection,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(lastAnimationEndTime,ReflectionData::SAVE_DOUBLE)
		CLASS_MEMBER(lastPlayedAnimation,ReflectionData::SAVE_STRING);
		CLASS_MEMBER(modelName,ReflectionData::SAVE_STRING);

		//somehow save ModelInstance
	END_DECLARATION
};

#endif
