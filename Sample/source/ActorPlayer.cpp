#include "stdafx.h"
#include "BaseFrame.h"
#include "ActorPlayer.h"
#include "PhysicsSystem.h"
#include "PhysicsVoxel.h"

//Right now there's no physics system and this actor is a pseudo physics object
//so here are the constants involved
static const float groundThreshold = .05;
static const float movementSpeed = 6.0f;
static const float gravity = -9.8f;
static const float jumpVelocity = 10.0f;

ActorPlayer::ActorPlayer() {
	//Start the player off in abouts the center
	position = vec3(512,512,0);
}
ActorPlayer::~ActorPlayer() {

}

/*void ActorPlayer::ReadInput(const vector<InputEvent> & events) {
	for (auto eve : events) {
		if ((eve.Event == InputEvent::KeyboardDown) && (eve.Key == ' ')) {
			//Check that the user is on the ground
			float posHeight = Game()->Voxels.GetPositionHeight(vec2(position.x,position.y));
			float relativeHeight = position.z-posHeight;
			if (relativeHeight <= groundThreshold) {
				//Apply upwards velocity
				velocity.z += jumpVelocity;
			}
		}
	}
}*/

//Update the position based off the most recent movement and direction vectors
void ActorPlayer::Update(float delta, float now) {
	//Your movement speed is the multiplier right now
	vec2 playerMotion = Game()->FirstPerson->GetMoveVector()*movementSpeed;
	
	//The side ways velocity is not effected by momentum
	//isn't that handy
	velocity.x = playerMotion.x;
	velocity.y = playerMotion.y;
    
    // Lets check if the controller wants us to jump
    if(Game()->FirstPerson->GetJumpRequested())
    {
        // Check that the user is on the ground
        float posHeight = Game()->Voxels.GetPositionHeight(vec2(position.x,position.y));
        float relativeHeight = position.z-posHeight;
        if (relativeHeight <= groundThreshold)
        {
            //Apply upwards velocity
            velocity.z += jumpVelocity;
        }
    }
    
	//Lets check if you're in the air or on the ground currently

	float posHeight = Game()->Voxels.GetPositionHeight(vec2(position.x,position.y));
	float relativeHeight = position.z-posHeight;
	if (relativeHeight > groundThreshold)
		//You're off the grund, apply gravity
		velocity.z += gravity*delta;
	else if (relativeHeight < -groundThreshold) {
		//You're below the ground, correct
		velocity.z = 0;
		position.z = posHeight;
	}
	else {
		//you're on the ground
		//velocity.z = 0;
	}
	//Next calculate a new position and check for validity (did you run into something?)
	vec3 newPos = position + velocity*delta;
	//Check if you're under terrain
	posHeight = Game()->Voxels.GetPositionHeight(vec2(newPos.x,newPos.y));
	relativeHeight = newPos.z-posHeight;
	if (relativeHeight > groundThreshold)
		//Position is good, allow it
		position = newPos;
	else if (relativeHeight < -groundThreshold) {
		//position is bad, leave it
		velocity = vec3();
	}
	else
		//You're on the ground
		position = newPos;
}