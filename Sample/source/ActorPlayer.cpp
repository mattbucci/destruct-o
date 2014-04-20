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
static const float jumpVelocity = 20.0f;

// Construct an actor player.  Weapon camera is setup for optimal weapon size
ActorPlayer::ActorPlayer()
    : PhysicsActor(vec3(2,2,6),500, GameFactions::FACTION_PLAYER), weapon(NULL), weaponCamera(GameCamera(40.0f))
{
	//Start the player off in abouts the center
	Position = (vec3(34,40,0));
    deltaPosition = 0.0;
	debug = true;
	debug_target_height = 0;
    weaponCamera.SetCameraView(vec3(0,0,0), glm::vec3(0,-1,0));
}
ActorPlayer::~ActorPlayer()
{
    // Make sure we delete the weapon model instance
    delete weapon;
}

// Create anything related to the actor
void ActorPlayer::Build()
{
    // Construct the weapon model instance
    weapon = Game()->Models()->NewInstance("player_weapon");
    weapon->GetTransform().Rotation() = glm::quat(vec3(0.5 * M_PI, 0.0, 0.0));
    weapon->GetTransform().Translation() = glm::vec3(0, -0.3, -1.95);
    weapon->PlayAnimation("Idle");
}

//Update the position based off the most recent movement and direction vectors
bool ActorPlayer::Update() {
	//Your movement speed is the multiplier right now
	vec2 moveVector = Game()->FirstPerson->GetMoveVector();
	vec2 playerMotion = moveVector*movementSpeed;
	
	//The side ways velocity is not effected by momentum
	//isn't that handy
	Velocity.x = playerMotion.x;
	Velocity.y = playerMotion.y;

	facingDirection = atan2(moveVector.y,moveVector.x);
	
    if(deltaPosition>200 && OnGround()) {
        //Let everyone know we walked
        Game()->Actors.ActorWalked.Fire([this](function<void(ActorPlayer*)> subscriber) {
            subscriber(this);
        });
		cout << "Player Position: " << Position.x << "," << Position.y << "," << Position.z << endl;
        deltaPosition -=200;
    } else {
        if(OnGround()) {
            deltaPosition+= sqrt(pow(Velocity.x,2)+pow(Velocity.y,2));
        }
    }
    
    // Lets check if the controller wants us to jump
    if(Game()->FirstPerson->GetJumpRequested())
    {
        // Check that the user is on the ground
		//and does not have any velocity upwards
        if (OnGround() && (Velocity.z < .025))
        {
            //Apply upwards velocity
            Velocity.z += jumpVelocity;
            
            //Let everyone know we jumped
            Game()->Actors.ActorJumped.Fire([this](function<void(ActorPlayer*)> subscriber) {
                subscriber(this);
            });
        }
    }

    //this code lets you fly

	//update our debug values
	if (!debug) Game()->FirstPerson->SetDebugHeight((int)Position.z);
	debug = Game()->FirstPerson->GetDebug();
	debug_target_height = Game()->FirstPerson->GetDebugHeight();

	if (debug && debug_target_height > Position.z+.5) {
		Velocity.z += .1f;
	}
	else if (debug && debug_target_height < Position.z-.5) {
		Velocity.z -= .1f;
	}
	else if(debug) {
		Velocity.z = 0.0f;
	}
	/*
	    if(!onGround) {
            //Let everyone know we landed
            PlayerLanded.Fire([this](function<void(ActorPlayer*)> subscriber) {
                subscriber(this);
            });
            onGround = true;
        }
		*/
	return PhysicsActor::Update();
}

// Draw the weapon of the actor player
void ActorPlayer::DrawWeapon(MaterialProgram *materialShader)
{
    if(weapon != NULL)
    {
        // Update the shader
        weaponCamera.Apply(materialShader);
        
        // Draw the model
        weapon->Update(SIMULATION_DELTA,Game()->Now());
		weapon->Draw(materialShader);
    }
}

GameCamera& ActorPlayer::WeaponCamera()
{
    return weaponCamera;
}