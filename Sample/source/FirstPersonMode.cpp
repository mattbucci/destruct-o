#include "FirstPersonMode.h"


FirstPersonMode::FirstPersonMode() {
	firstPersonEnabled = false;
}
FirstPersonMode::~FirstPersonMode() {

}

//Get the normalized direction the player is currently looking
vec3 FirstPersonMode::GetLookVector() {
	return lookVector;
}
//Get the movement vector for the direction and magnitude
//the player is currently moving. Magnitude is between 0 (not moving) and 1 (moving full speed)
vec2 FirstPersonMode::GetMoveVector() {
	return moveVector;
}

vec2 FirstPersonMode::GetAngleVector() {
	return aggregateMouseVector;
}

//Enable or disable first person mode 
void FirstPersonMode::Enable(bool enableFirstPerson) {
#ifndef __MOBILE__
	//A neat little feature of SDL
	//causes all input events to be deltas
	SDL_SetRelativeMouseMode((SDL_bool)enableFirstPerson);
#endif
	firstPersonEnabled = enableFirstPerson;
}
//Update the looking direction based off input events
void FirstPersonMode::UpdateLookingDirection(set<int> pressedKeys, vector<InputEvent> input) {
	if (!firstPersonEnabled)
		return;

	//Desktop method of calculating move vector
	moveVector = vec2(0,0);
	if (pressedKeys.find('w') != pressedKeys.end())
		moveVector.x = 1;
	else if (pressedKeys.find('s') != pressedKeys.end())
		moveVector.x = -1;

	if (pressedKeys.find('a') != pressedKeys.end())
		moveVector.y = 1;
	else if (pressedKeys.find('d') != pressedKeys.end())
		moveVector.y = -1;

	//Sum up the mouse deltas into the current looking vector
	//Mouse sensitivity constants for now
	static const float mouseXSensitivity = 1.0f/6.0f;
	static const float mouseYSensitivity = 1.0f/6.0f;
	vec2 mouseDeltaSum;
	for (auto e : input) {
		if (e.Event == InputEvent::MouseMove) {
			mouseDeltaSum -= vec2(e.RelX,e.RelY);
		}
			
	}
	//No update at this time
	if ((mouseDeltaSum == vec2()) && (aggregateMouseVector != vec2()))
		return;
	//Now use the current mouse delta to build an aggregate
	aggregateMouseVector += mouseDeltaSum*vec2(mouseXSensitivity,mouseYSensitivity);

	//Limit the aggregate appropriately
	aggregateMouseVector.x = glm::mod(aggregateMouseVector.x,360.0f);
	if (aggregateMouseVector.y >= 75)
		aggregateMouseVector.y = 75;
	if (aggregateMouseVector.y <= -60)
		aggregateMouseVector.y = -60;
	//Calculate the looking vector from the new aggregate angle vector
	//this part adapted from a previous FPS engine I wrote, so its a bit contrived
	float f = aggregateMouseVector.x/180.f*M_PI;
	float t = (90.0f-aggregateMouseVector.y)/180.0f*M_PI;
	lookVector.x = sin(t)*cos(f);
	lookVector.y = sin(t)*sin(f);
	lookVector.z = cos(t);
	lookVector = glm::normalize(lookVector); //IIRC this isn't necessary
}
//In the future this will be used to draw the virtual joysticks on mobile
void FirstPersonMode::Draw() {

}