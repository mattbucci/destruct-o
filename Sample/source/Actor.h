
#pragma once

#include "stdafx.h"

class ActorSystem;
class BaseFrame;

class Actor : public Savable {
	bool valid;
protected:
	//Safely erase this actor during the next update cycle
	void Destroy();

	//Retrieve the game object
	BaseFrame * Game();
public:
	//Automatically registers this actor
	Actor();
	virtual ~Actor();

	//Update this actor with the delta and current time
	//returns false if the actor should be destroyed
	virtual bool Update(float delta, float now);

	//Draw this actor
	virtual void Draw();

	CLASS_DECLARATION(ActorSystem)
	END_DECLARATION
};