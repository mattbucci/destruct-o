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

	//Update the position based off the most recent movement and direction vectors
	void Update(float delta, float now) override;
    //player events
    GameEvent<void(ActorPlayer*)> PlayerJumped;
    GameEvent<void(ActorPlayer*)> PlayerLanded;
    GameEvent<void(ActorPlayer*)> PlayerWalked;
};