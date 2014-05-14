#include "stdafx.h"
#include "BaseFrame.h"
#include "ActorPlayer.h"
#include "PhysicsSystem.h"
#include "PhysicsVoxel.h"
#include "GLEffectProgram.h"

#include "ModelGroup.h"
#include "ModelInstance.h"
#include "Model.h"

#include "VoxEngine.h"
#include "FirstPersonMode.h"


CLASS_SAVE_CONSTRUCTOR(ActorPlayer);

//Right now there's no physics system and this actor is a pseudo physics object
//so here are the constants involved
static const float movementSpeed = 6.0f;
static const float jumpVelocity = 20.0f;

// Construct an actor player.  Weapon camera is setup for optimal weapon size
ActorPlayer::ActorPlayer()
	: PhysicsActor(GameFactions::FACTION_PLAYER), weaponCamera(GameCamera(40.0f)), currentWeapon(NULL)
{
	//setup defaults
	Size = vec3(2,2,6);
	//start the user with a full charge
	life = maxLife = 200;
	energyPool = maxEnergyPool = 100;
	
	//Start the player off in abouts the center
	Position = (vec3(0,0,0));
	deltaPosition = 0.0;
	debug = true;
	debug_target_height = 0;
	weaponCamera.SetCameraView(vec3(0,0,0), glm::vec3(0,-1,0), 1000);
	currentWeaponId = 0;
}

ActorPlayer::~ActorPlayer()
{
	VoxEngine::SynchronousTask.RequestTask([this]()
	{
		for (auto weapon : weapons)
			delete weapon;
		weapons.clear();
	});
}


float ActorPlayer::GetCharge() {
	return energyPool;
}

float ActorPlayer::GetMaxCharge() {
	return maxEnergyPool;
}



//When an actor is loaded
//handles building weapons and model and such
void ActorPlayer::Load(Json::Value & parentValue, LoadData & loadData) {
	//Load this class' data
	PhysicsActor::Load(parentValue,loadData);
	//Setup the weapons
	VoxEngine::SynchronousTask.RequestTask([this]() {
		// Create the weapons only if none were loaded
		if (weapons.size() <= 0) {
			//Setup weapons
			weapons.push_back(Game()->Actors.BuildWeapon("playerpulselaser.json",this));
			weapons.push_back(Game()->Actors.BuildWeapon("playerlasercannon.json",this));
		}

		// Set initial weapon to pulse laser
		setWeapon(currentWeaponId);
	});
}


void ActorPlayer::setWeapon(int weaponId)
{
	this->currentWeaponId = weaponId;
	// Construct the weapon model instance
	if(!model)
	{
		setModel("player_weapon");
	}
	model->GetTransform().Translation() = glm::vec3(0, -0.3, -1.95);
	
	// Based on which weapon it is, select a reticle
	if(weaponId == 0)
	{
		Game()->GetHUD()->SetReticle("hud/reticle_sniper.png", glm::vec2(48, 48));
	} else
	{
		Game()->GetHUD()->SetReticle("hud/reticle.png", glm::vec2(48, 48));
	}
	
	//Save weapon
	this->currentWeapon = weapons[weaponId];
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
		return PhysicsActor::Update();
	
	// Get the movement vector from the first person controller
	vec2 moveVector = Game()->FirstPerson->GetMoveVector();
	
	// Calculate the direction we are facing
	facingDirection = atan2(moveVector.y, moveVector.x);
	
	// Calculate the magnitude of the movement vector (are we sprinting)
	float magnitude = glm::length(moveVector);
	
	// Check if we should switch weapons
	if(Game()->FirstPerson->GetSwitchWeaponRequested())
		setWeapon((currentWeaponId+1) % weapons.size());
	
	// Update the weapon
	currentWeapon->Update(Game()->FirstPerson->GetLookVector(), weaponPos);

	// Forward whether or not we want to shoot to the player's weapon animation controller
	bool r = currentWeapon->HoldingTrigger(Game()->FirstPerson->GetTriggerPulled());
	model->Controller()->SetBoolean("firing", r);


	// if we fired the weapon, update the state machine
	//but only if its not the pulseLaser, no idea why
	if(r && (currentWeaponId == 0))
		model->Update(SIMULATION_DELTA, Game()->Now());
	
	// Forward the weapon mode to the controller
	model->Controller()->SetBoolean("mode", (currentWeaponId == 0) ? true : false);
	
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
	//Run autojump?
	if (VoxEngine::SavedDeviceData.GameOptions.Autojump > 0) {
		//Autojump
		if (glm::length(moveVector) > .15) {
			//Only jump if you're touching ground
			bool touchingGround = (OnGround() && Velocity.z < .2);
			//Check if you've got terrain (cliff/hill) directly in front of you
			//Check just a bit above your feet
			float feetHeight = Position.z-Size.z/2.0f;
			float checkHeight = feetHeight+.5f;
			float rayLength;
			vec3 surfaceNormal;
			vec3 traceDirection = vec3(moveVector,0);
			if (Game()->Voxels.RaytraceToTerrain(vec3(Position.x,Position.y,checkHeight),traceDirection,rayLength,surfaceNormal)) {
				if (rayLength < 1.5) {
					//Check the height of the given location
					//add .15 to go pas the surface and onto the voxel itself
					vec3 upcomingTerrain = vec3(Position.x,Position.y,checkHeight) + traceDirection*(rayLength+.15f);
					float upcomingHeight = Game()->Voxels.GetPositionHeight(vec2(upcomingTerrain));
					//Now check if you're too low
					if (feetHeight+.15 < upcomingHeight) {
						if (touchingGround) {
							//Ok lets jump up
							//Playing some kind of jump animation would be A+
							Velocity.z += min(15*(upcomingHeight-feetHeight),20.0f);
							//jump!
						}
				
					}
				}
			}
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

	
//Get modifiers for the selected weapon
//A damage factor of 0 indicates the weapon is locked
WeaponModifiers * ActorPlayer::GetModifiers(int weaponId) {
	_ASSERTE(weaponId > 0 && weaponId < weapons.size());
	return &weapons[weaponId]->Modifiers;
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