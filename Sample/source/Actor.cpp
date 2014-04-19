#include "stdafx.h"
#include "Actor.h"
#include "BaseFrame.h"
#include "Frames.h"

CLASS_SAVE_CONSTRUCTOR(Actor);

Actor::Actor() {
	valid = true;
}
Actor::~Actor() {
}

//Safely erase this actor during the next update cycle
void Actor::Destroy() {
	valid = false;
}


//Update this actor with the delta and current time
bool Actor::Update() {
	return !valid;
}

//Draw this actor
void Actor::Draw() {

}