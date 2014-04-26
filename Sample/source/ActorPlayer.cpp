#include "stdafx.h"
#include "BaseFrame.h"
#include "ActorPlayer.h"
#include "PhysicsSystem.h"
#include "PhysicsVoxel.h"
#include "GLEffectProgram.h"


CLASS_SAVE_CONSTRUCTOR(ActorPlayer);

//Right now there's no physics system and this actor is a pseudo physics object
//so here are the constants involved
static const float groundThreshold = .05f;
static const float movementSpeed = 6.0f;
static const float gravity = -9.8f;
static const float jumpVelocity = 20.0f;
//Other player specific constants
static const float energyPerSecond = 5.0f;

// Construct an actor player.  Weapon camera is setup for optimal weapon size
ActorPlayer::ActorPlayer()
    : PhysicsActor(vec3(2,2,6),500, GameFactions::FACTION_PLAYER), weaponCamera(GameCamera(40.0f)), currentWeapon(NULL), pulseLaser(NULL), laserCannon(NULL)
{
	//Start the player off in abouts the center
	Position = (vec3(34,40,0));
    deltaPosition = 0.0;
	debug = true;
	debug_target_height = 0;
    weaponCamera.SetCameraView(vec3(0,0,0), glm::vec3(0,-1,0));
    
	//Start the user with full charge
	energyPool = 100;
	maxEnergyPool = 100;
}

ActorPlayer::~ActorPlayer()
{
    /*VoxEngine::SynchronousTask.RequestTask([this]()
    {
        delete pulseLaser;
        delete laserCannon;
    });*/
}

float ActorPlayer::GetCharge() {
    return energyPool;
}

float ActorPlayer::GetMaxCharge() {
	return maxEnergyPool;
}

// Create anything related to the actor
void ActorPlayer::Build()
{
    // Create the weapons
    pulseLaser = new WeaponPulseLaser(this);
    laserCannon = new WeaponLaserCannon(this);
    
    // Set initial weapon to pulse laser
    setWeapon(pulseLaser);
}

void ActorPlayer::setWeapon(Weapon * weapon)
{
    // Construct the weapon model instance
    if(!model)
    {
        setModel("player_weapon");
    }
    model->GetTransform().Translation() = glm::vec3(0, -0.3, -1.95);
    
    // Based on which weapon it is, select a reticle
    if(weapon == laserCannon)
    {
        Game()->GetHUD()->SetReticle("hud/reticle_sniper.png", glm::vec2(48, 48));
    } else
    {
        Game()->GetHUD()->SetReticle("hud/reticle.png", glm::vec2(48, 48));
    }
    
    //Save weapon
    this->currentWeapon = weapon;
}

void ActorPlayer::onDeath()
{
	vulnerable	 = false;
	cout << "THE PLAYER HAS DIED!\n";
}


//Update the position based off the most recent movement and direction vectors
bool ActorPlayer::Update()
{
    // If the weapon is null, don't do shit
    if(currentWeapon == NULL)
    {
        return false;
    }
    
	// Get the movement vector from the first person controller
	vec2 moveVector = Game()->FirstPerson->GetMoveVector();
    
    // Calculate the direction we are facing
	facingDirection = atan2(moveVector.y, moveVector.x);
    
    // Calculate the magnitude of the movement vector (are we sprinting)
    float magnitude = glm::length(moveVector);
    
    // Check if we should switch weapons
    if(Game()->FirstPerson->GetSwitchWeaponRequested())
    {
        if(currentWeapon == laserCannon)
        {
            setWeapon(pulseLaser);
        } else
        {
            setWeapon(laserCannon);
        }
    }
    
    // Update the weapon
	currentWeapon->Update(Game()->FirstPerson->GetLookVector(), weaponPos);
    
    // Recharge our energy pool
    energyPool = glm::clamp(energyPool + (energyPerSecond * (float) SIMULATION_DELTA), 0.0f, 100.0f);
    
    // The firing setting depends on the weapon
    if(currentWeapon == pulseLaser)
    {
        // Forward the potential shoot request
        currentWeapon->HoldingTrigger(Game()->FirstPerson->GetTriggerPulled());
        
        // Forward whether or not we want to shoot to the player's weapon animation controller
        model->Controller()->SetBoolean("firing", Game()->FirstPerson->GetTriggerPulled());
    } else
    {
        // Forward whether or not we want to shoot to the player's weapon animation controller
        bool r = currentWeapon->HoldingTrigger(Game()->FirstPerson->GetTriggerPulled());
        model->Controller()->SetBoolean("firing", r);
        
        // if we fired the weapon, update the state machine
        if(r)
        {
            model->Update(SIMULATION_DELTA, Game()->Now());
        }
    }
    
    // Forward the weapon mode to the controller
    model->Controller()->SetBoolean("mode", (currentWeapon == laserCannon) ? true : false);
    
    // Forward the movement speed to the player's weapon animation controller
    model->Controller()->SetFloat("speed", OnGround() ? magnitude : 0.0f);
    
	// Use the movement vector to set the velocity of the player's physics object
	Velocity.x = moveVector.x * movementSpeed;
	Velocity.y = moveVector.y * movementSpeed;
	
    // Have we walked more than 200 units?
    if(deltaPosition > 200 && OnGround())
    {
        // Fire the actor walked event
        Game()->Actors.ActorWalked.Fire([this](function<void(ActorPlayer*)> subscriber)
        {
            subscriber(this);
        });
        
		//cout << "Player Position: " << Position.x << "," << Position.y << "," << Position.z << endl;
        deltaPosition -=200;
    }
    
    // If we haven't, check if we should add more distance to our odometer
    else
    {
        if(OnGround())
        {
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
            // Apply upwards velocity
            Velocity.z += jumpVelocity;
            
            // First the event that we'e
            Game()->Actors.ActorJumped.Fire([this](function<void(ActorPlayer*)> subscriber)
            {
                subscriber(this);
            });
        }
    }

    // Return the result of the update of the super class
	return PhysicsActor::Update();
}

void ActorPlayer::Draw(MaterialProgram * materialShader)
{

}

// Draw the weapon of the actor player
void ActorPlayer::DrawWeapon(MaterialProgram *materialShader)
{
    // If we have a weapon
    if (model != NULL)
    {
		//testWeapon.Update(Game()->FirstPerson->GetLookVector(),Position);

        // Update the shader
        weaponCamera.Apply(materialShader);
        
        // Draw the model
        model->Update(SIMULATION_DELTA, Game()->Now());
        model->Draw(materialShader);

		// Find the bone attached to the muzzle of the player weapon
        const Node * n = model->Controller()->Bones()["b_muzzle"];
        mat4 globalTransform = model->GetTransform().TransformMatrix() * n->TransformMatrix();

		vec3 fVector(.15,0,0);
		//Calculate the position along the muzzle
		weaponBonePos = vec3(globalTransform * vec4(fVector,1.0));
		//Construct the MVP for the altered perspective
		mat4 aModel = materialShader->Model.GetMatrix();
		mat4 aView, aProjection;
		materialShader->Camera.CopyMatricies(&aView,&aProjection);
		//Save the altered MVP
		alteredMVP = aProjection * (aView * aModel);
    }
}

//Draw the effects of said weapon
void ActorPlayer::Draw(GLEffectProgram * shader)
{
	//Construct the MVP from the world perspective
	mat4 aModel = shader->Model.GetMatrix();
	mat4 aView, aProjection;
	shader->Camera.CopyMatricies(&aView,&aProjection);
	mat4 worldMVP = aProjection * (aView * aModel);
	mat4 inverseWorldMVP = glm::inverse(worldMVP);
	//Now transform the position
	vec4 screen = alteredMVP * vec4(weaponBonePos,1.0f);
	weaponPos = vec3((inverseWorldMVP ) * screen);
	//Save this position


	currentWeapon->DrawWeapon(shader,Game()->FirstPerson->GetLookVector(),weaponPos);
}

GameCamera& ActorPlayer::WeaponCamera()
{
    return weaponCamera;
}