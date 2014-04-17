#include "stdafx.h"
#include "Actor.h"
#include "BaseFrame.h"
#include "Frames.h"

CLASS_SAVE_CONSTRUCTOR(Actor);

Actor::Actor() {
	valid = true;
	//Assume if an actor is created the frame is the game frame
	Game()->Actors.Register(this);
}
Actor::~Actor() {
	//Assume if an actor is created the frame is the game frame
	Game()->Actors.Unregister(this);
}



//Update this actor with the delta and current time
bool Actor::Update() {
	return valid;
}

//Draw this actor
void Actor::Draw() {

}