#pragma once

#include "Actor.h"
#include "InputEvent.h"
#include "GameEvent.h"

class ActorPlayer : public Actor {
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
		INHERITS_FROM(Actor)
		CLASS_MEMBER(debug,ReflectionData::SAVE_BOOL);
		CLASS_MEMBER(debug_target_height,ReflectionData::SAVE_FLOAT);
	END_DECLARATION
};