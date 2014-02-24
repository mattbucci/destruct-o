
#pragma once

class ActorSystem;
class BaseFrame;

class Actor {
protected:
	//The actor system that this actor uses
	ActorSystem * actorSystem;

	//Every actor has position
	vec3 position;
	//Right now actors are pseudo physical
	//Later a physics system will take over
	vec3 velocity;
	//Any acceleration forces applied upon the actor
	//in this case, just gravity
	vec3 acceleration;
    
    //trigger for landing sound
    bool onGround;
    //delta since last walk fired
    double deltaPosition;

	//Retrieve the game object
	BaseFrame * Game();
public:
	//Automatically registers this actor
	Actor();
	virtual ~Actor();

	//Update this actor with the delta and current time
	virtual void Update(float delta, float now);

	//Retrieve the current position of the actor
	vec3 GetPosition();

	//Draw this actor
	virtual void Draw();
};