
#pragma once

class ActorSystem;
class BaseFrame;

class Actor {
protected:
	//The actor system that this actor uses
	ActorSystem * actorSystem;

	//Retrieve the game object
	BaseFrame * Game();
public:
	//Automatically registers this actor
	Actor();
	virtual ~Actor();

	//Update this actor with the delta and current time
	virtual void Update(double delta, double now);

	//Draw this actor
	virtual void Draw();
};