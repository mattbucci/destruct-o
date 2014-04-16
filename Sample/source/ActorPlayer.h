#pragma once

#include "PhysicsActor.h"
#include "GameEvent.h"

class ActorPlayer : public PhysicsActor {

	
	//delta since last walk fired
	double deltaPosition;
public:
	ActorPlayer();
	~ActorPlayer();

	//The player is the only actor which consumes input events
	//void ReadInput(const vector<InputEvent> & events);
	bool debug;
	float debug_target_height;
	//Update the position based off the most recent movement and direction vectors
	bool Update(float delta, float now) override;
    //player events
    vec3 GetPosition();
    GameEvent<void(ActorPlayer*)> PlayerJumped;
    GameEvent<void(ActorPlayer*)> PlayerLanded;
    GameEvent<void(ActorPlayer*)> PlayerWalked;


	CLASS_DECLARATION(ActorPlayer)
		INHERITS_FROM(PhysicsActor)
		CLASS_MEMBER(debug,ReflectionData::SAVE_BOOL);
		CLASS_MEMBER(deltaPosition,ReflectionData::SAVE_DOUBLE)
		CLASS_MEMBER(debug_target_height,ReflectionData::SAVE_FLOAT);
	END_DECLARATION
};