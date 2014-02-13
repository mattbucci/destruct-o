#include "FirstPersonMode.h"


FirstPersonMode::FirstPersonMode()
{
	firstPersonEnabled = false;
    jumpRequested = false;
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
	//Rotate the move vector
	//by the facing position
	//Currently uses a mat4 which is a total waste of CPU, but whatever
	return vec2(glm::rotate(aggregateMouseVector.x,vec3(0,0,1))*vec4(moveVector,0,0));
}

vec2 FirstPersonMode::GetAngleVector() {
	return aggregateMouseVector;
}

bool FirstPersonMode::GetJumpRequested(bool clearFlag)
{
    bool _t = jumpRequested;
    if(clearFlag)
    {
        jumpRequested = false;
    }
    return _t;
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
void FirstPersonMode::ReadInput(set<Sint64> pressedKeys, vector<InputEvent> input) {
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
    
    // Did the user request jump?
    if (pressedKeys.find(' ') != pressedKeys.end())
    {
        // User requested jump
        jumpRequested = true;
    }

	//Sum up the mouse deltas into the current looking vector
	//Mouse sensitivity constants for now
	static const float mouseXSensitivity = 1.0f/6.0f;
	static const float mouseYSensitivity = 1.0f/6.0f;
	vec2 mouseDeltaSum;
	for (auto e : input)
    {
        // If the input event is that the mouse moved
		if (e.Event == InputEvent::MouseMove)
        {
			mouseDeltaSum -= vec2(e.RelX,e.RelY);
		}
	}
    
    // If we have an active joystick, override keyboard with joystick
    if(joystick)
    {
        // Get the x and y axes of the left joystick. (Xbox 360 gamepad)
        Sint32 axisX = SDL_JoystickGetAxis(joystick, 0);
        Sint32 axisY = SDL_JoystickGetAxis(joystick, 1);
        
        // Get the x and y axes of the right joystick. (Xbox 360 gamepad)
        Sint32 axisLX = SDL_JoystickGetAxis(joystick, 3);
        Sint32 axisLY = SDL_JoystickGetAxis(joystick, 4);
        
        // Get the button to check if we want jump requests (left bumper)
        if(SDL_JoystickGetButton(joystick, 4))
        {
            jumpRequested = true;
        }
        
        // Check if we should override the movement vector
        if(axisX != 0 || axisY != 0)
        {
            // Override motion vector
            moveVector.x = static_cast<float>(-axisY) / 32767.0;
            moveVector.y = static_cast<float>(-axisX) / 32767.0;
        }
        
        // Check if we should override the mouse delta sum
        if(axisLX != 0 || axisLY != 0)
        {
            // Override motion vector
            mouseDeltaSum.x = (static_cast<float>(-axisLX) / 32767.0) * 16.0;
            mouseDeltaSum.y = (static_cast<float>(-axisLY) / 32767.0) * 16.0;
        }
    }
    
	//No update at this time
	if ((mouseDeltaSum == vec2()) && (aggregateMouseVector != vec2()))
		return;
    
	//Now use the current mouse delta to build an aggregate
	aggregateMouseVector += mouseDeltaSum*vec2(mouseXSensitivity,mouseYSensitivity);

	//Limit the aggregate appropriately
	aggregateMouseVector.x = glm::mod(aggregateMouseVector.x,360.0f);
    aggregateMouseVector.y = clamp<float>(aggregateMouseVector.y, -60.0f, 75.0f);
    
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
void FirstPersonMode::Draw(double width, double height, GL2DProgram * shader)
{

}