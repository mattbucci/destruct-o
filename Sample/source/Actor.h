
#pragma once

#include "stdafx.h"

class ActorSystem;
class BaseFrame;

class Actor : public Savable {
	bool valid;
protected:
	//Safely erase this actor during the next update cycle
	void Destroy();


public:
	//Automatically registers this actor
	Actor();
	virtual ~Actor();

	//Update this actor 
	//returns true if the actor should be destroyed
	//otherwise return false
	virtual bool Update();

	//Draw this actor
	virtual void Draw();

	CLASS_DECLARATION(ActorSystem)
		CLASS_MEMBER(valid,ReflectionData::SAVE_BOOL)
	END_DECLARATION
};