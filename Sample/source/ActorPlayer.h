#pragma once

#include "PhysicsActor.h"
#include "GameEvent.h"

class ActorPlayer : public PhysicsActor {

	//trigger for landing sound
	bool onGround;
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
	void Update(float delta, float now) override;
    //player events
    GameEvent<void(ActorPlayer*)> PlayerJumped;
    GameEvent<void(ActorPlayer*)> PlayerLanded;
    GameEvent<void(ActorPlayer*)> PlayerWalked;

	CLASS_DECLARATION(ActorPlayer)
		INHERITS_FROM(PhysicsActor)
		CLASS_MEMBER(onGround,ReflectionData::SAVE_BOOL)
		CLASS_MEMBER(deltaPosition,ReflectionData::SAVE_DOUBLE)
	END_DECLARATION
};