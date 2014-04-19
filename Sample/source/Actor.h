
#pragma once

#include "stdafx.h"

class ActorSystem;
class BaseFrame;
class ModelInstance;
class MaterialProgram;
class PhysicsActor;

class Actor : public Savable {
	bool valid;

	//The last time an animation ended (or looped)
	double lastAnimationEndTime;

	//The name of the model being used
	string modelName;

	//The last animation being played
	string lastPlayedAnimation;

	//Direct access to the model is unavailable
	//this is temporary
	//when the animation system is replaced
	//we can reallow direct access
	ModelInstance * model;
	//This is a hack while modelinstance is private
	friend class PhysicsActor;
protected:
	//When an actor is loaded
	//handles rebuilding the model instance
	virtual void Load(Json::Value & parentValue, LoadData & loadData) override;

	//Safely erase this actor during the next update cycle
	void Destroy();

	//Play an animation
	//also allows animationRunning() to work
	void playAnimation(string animationName);

	//Set the current model
	//if modelName is invalid
	//model will still be NULL
	void setModel(string modelName);

	//Checks if an animation is running
	//true if the animation is running
	//false if the animation has looped
	bool animationRunning();

	//Facing direction for the model mostly
	//in degrees
	float facingDirection;
public:
	//Automatically registers this actor
	Actor();
	virtual ~Actor();

	//Update this actor 
	//returns true if the actor should be destroyed
	//otherwise return false
	virtual bool Update();

	//Draw this actor
	virtual void Draw(MaterialProgram * materialShader);

	CLASS_DECLARATION(Actor)
		CLASS_MEMBER(valid,ReflectionData::SAVE_BOOL)
		CLASS_MEMBER(facingDirection,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(lastAnimationEndTime,ReflectionData::SAVE_DOUBLE)
		CLASS_MEMBER(lastPlayedAnimation,ReflectionData::SAVE_STRING);
		CLASS_MEMBER(modelName,ReflectionData::SAVE_STRING);

		//somehow save ModelInstance
	END_DECLARATION
};