#include "stdafx.h"
#include "Actor.h"
#include "BaseFrame.h"
#include "Frames.h"
Actor::Actor() {
	//Assume if an actor is created the frame is the game frame
	Game()->Actors.Register(this);
}
Actor::~Actor() {
	//Assume if an actor is created the frame is the game frame
	Game()->Actors.Unregister(this);
}

BaseFrame * Actor::Game() {
	//Assume actors are only created when the current system is the game frame
	return (BaseFrame*)CurrentSystem;
}

//Update this actor with the delta and current time
void Actor::Update(double delta, double now) {

}

//Draw this actor
void Actor::Draw() {

}