#include "stdafx.h"
#include "BaseFrame.h"
#include "ActorPlayer.h"
#include "PhysicsSystem.h"
#include "PhysicsVoxel.h"

CLASS_SAVE_CONSTRUCTOR(ActorPlayer);

//Right now there's no physics system and this actor is a pseudo physics object
//so here are the constants involved
static const float groundThreshold = .05f;
static const float movementSpeed = 6.0f;
static const float gravity = -9.8f;
static const float jumpVelocity = 10.0f;

ActorPlayer::ActorPlayer() {
	//Start the player off in abouts the center
	position = vec3(218,189,0);
    deltaPosition = 0.0;
    onGround = true;
	debug = true;
	debug_target_height = 0;
}
ActorPlayer::~ActorPlayer() {

}

//Update the position based off the most recent movement and direction vectors
void ActorPlayer::Update(float delta, float now) {
	//Your movement speed is the multiplier right now
	vec2 playerMotion = Game()->FirstPerson->GetMoveVector()*movementSpeed;
	
	//The side ways velocity is not effected by momentum
	//isn't that handy
	velocity.x = playerMotion.x;
	velocity.y = playerMotion.y;
	
    if(deltaPosition>200 && onGround) {
        //Let everyone know we walked
        PlayerWalked.Fire([this](function<void(ActorPlayer*)> subscriber) {
            subscriber(this);
        });
		cout << "Player Position: " << position.x << "," << position.y << "," << position.z << endl;
        deltaPosition -=200;
    } else {
        if(onGround) {
            deltaPosition+= sqrt(pow(velocity.x,2)+pow(velocity.y,2));
        }
    }
    
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
            
            //Let everyone know we jumped
            PlayerJumped.Fire([this](function<void(ActorPlayer*)> subscriber) {
                subscriber(this);
            });
        }
    }

    //this code lets you fly

	//update our debug values
	if (!debug) Game()->FirstPerson->SetDebugHeight((int)position.z);
	debug = Game()->FirstPerson->GetDebug();
	debug_target_height = Game()->FirstPerson->GetDebugHeight();

	if (debug && debug_target_height > position.z+.5) {
		velocity.z += .1f;
	}
	else if (debug && debug_target_height < position.z-.5) {
		velocity.z -= .1f;
	}
	else if(debug) {
		velocity.z = 0.0f;
	}


	//Lets check if you're in the air or on the ground currently
	float posHeight = Game()->Voxels.GetPositionHeight(vec2(position.x,position.y));
	float relativeHeight = position.z-posHeight;
	if (relativeHeight > groundThreshold && !debug) {
		//You're off the grund, apply gravity
		velocity.z += gravity*delta;
        onGround = false;
	}
    else if (relativeHeight < -groundThreshold && !debug) {
		//You're below the ground, correct
		if (velocity.z < 0)
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
	if (relativeHeight > groundThreshold || debug)
		//Position is good, allow it
		position = newPos;
	else if (relativeHeight < -groundThreshold) {
		//position is bad, cancel movement that's not directly upwards
		//cancel velocity that's not directly up
		float v = glm::dot(velocity,vec3(0,0,1));
		if (v < 0)
			v = 0;
		velocity = v*vec3(0,0,1);
		//Apply the new only-up velocity to the player
		position += velocity*delta;
	}
	else {
		//You're on the ground
		position = newPos;
        if(!onGround) {
            //Let everyone know we landed
            PlayerLanded.Fire([this](function<void(ActorPlayer*)> subscriber) {
                subscriber(this);
            });
            onGround = true;
        }
    }
}
vec3 ActorPlayer::GetPosition(){
    return position;
}