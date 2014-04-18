#include "stdafx.h"
#include "HUD.h"
#include "GL2DProgram.h"
#include "VoxEngine.h"
#include "BaseFrame.h"
#include "ActorPlayer.h"

#define MINIMAP_SIZE 200
#define MINIMAP_DOT_SIZE 20
#define DAMAGE_INDICATOR_SIZE 100

#define MOBILE_MINIMAP_SIZE 200
#define MOBILE_MINIMAP_DOT_SIZE 20
#define MOBILE_DAMAGE_INDICATOR_SIZE 100

#define DAMAGE_INDICATOR_TIME 1

HUD::HUD(BaseFrame* baseFrame) :
#ifdef __MOBILE__
	damageIndicator(Rect(0,0,MOBILE_DAMAGE_INDICATOR_SIZE,MOBILE_DAMAGE_INDICATOR_SIZE),"hud/arrow.png",vec4(7,.2,.2,.66)),
	minimapDot(Rect(0,0,MOBILE_MINIMAP_DOT_SIZE,MOBILE_MINIMAP_DOT_SIZE),"hud/dot.png",vec4(1,0,0,1)),
	minimapBackground(Rect(0,0,MOBILE_MINIMAP_SIZE,MOBILE_MINIMAP_SIZE),"hud/minimap.png", vec4(1, 1, 1, .5))
#else
	damageIndicator(Rect(0,0,DAMAGE_INDICATOR_SIZE,DAMAGE_INDICATOR_SIZE),"hud/arrow.png",vec4(7,.2,.2,.66)),
	minimapDot(Rect(0,0,MINIMAP_DOT_SIZE,MINIMAP_DOT_SIZE),"hud/dot.png",vec4(1,0,0,1)),
	minimapBackground(Rect(0,0,MINIMAP_SIZE,MINIMAP_SIZE),"hud/minimap.png", vec4(1, 1, 1, .5))
#endif
{
		this->baseFrame = baseFrame;
		minimapScale = 1.0f;
}

//Since the hud is purely visual
//there is no point in updating it at 100hz
//it might as well update as its drawn
void HUD::DrawAndUpdate(GL2DProgram * shader, vec2 viewPortSize) {

	//Update References
	player = baseFrame->Actors.Player();
	fps = baseFrame->FirstPerson;
	actors = baseFrame->Physics.GetActors();

	//Grab Player Position and Look Vector
	float px = player->GetPosition().x;
	float py = player->GetPosition().y;
	float playerAngle = fps->GetAngleVector().x;

	// ---------------------------------
	// |||||||||||| MINIMAP ||||||||||||
	// ---------------------------------

	//Preserve the current matrix
	shader->Model.PushMatrix();

	vec2 minimapLoc;
#ifdef __MOBILE__
	minimapLoc = vec2(40.0f, 40.0f);
#else
	minimapLoc = vec2(0.0f, viewPortSize.y - MINIMAP_SIZE);
#endif
	shader->Model.Translate(minimapLoc.x, minimapLoc.y, 0);

	//Apply Matrix Transformations
	shader->Model.Apply();

	//Draw Background
	minimapBackground.Draw(shader);

	//Translate Position to Exact Center of Minimap
	shader->Model.Translate(MINIMAP_SIZE * .5f, MINIMAP_SIZE * .5f, 0);

	//Draw Player @ Center of Minimap
	shader->Model.PushMatrix();
	shader->Model.Translate(-MINIMAP_DOT_SIZE * .5f, -MINIMAP_DOT_SIZE * .5f,100.0f);
	shader->Model.Apply();
	minimapDot.SetColor(vec4(0, 0, 0, 1));
	minimapDot.Draw(shader);
	shader->Model.PopMatrix();

	int s = actors->size();
	for(int i = 0; i < s; i++) {
		//Ignore the Player
		if((*actors)[i] == player)
			continue;

		//Grab Actor Position
		float x = (*actors)[i]->GetPosition().x;
		float y = (*actors)[i]->GetPosition().y;

		//Calculate Actor Angle Relative to Player
		float actorAngle = 180 / M_PI * atan2(y - py, x - px);
		//Calculate Actor Distance Relative to Player
		float actorDistance = sqrtf((y - py) * (y - py) + (x - px) * (x - px)) * minimapScale;

		//Intensity fade near edges of Minimap
		float intensity = min(1.0f, (MINIMAP_SIZE*.5f - actorDistance - MINIMAP_DOT_SIZE*.5f) / (MINIMAP_SIZE * .064f));
		minimapDot.SetColor(vec4(1,0,0,intensity));

		//Hacky way to prevent redraw of player dot.
		//Will also omit players directly below player
		//This should be done in a smarter way later.

		//Translate to Relative Location on Minimap
		shader->Model.PushMatrix();
		shader->Model.Rotate(playerAngle - actorAngle, vec3(0, 0, 1));
		shader->Model.Translate(vec3(0, -actorDistance, 0));
		shader->Model.Rotate(playerAngle - actorAngle, vec3(0, 0, -1));
		shader->Model.Translate(vec3(-MINIMAP_DOT_SIZE * .5, -MINIMAP_DOT_SIZE * .5, 0));
		shader->Model.Apply();

		//Draw the Actor as a Dot on Map
		minimapDot.Draw(shader);

		//Reset to Center of Minimap
		shader->Model.PopMatrix();
	}

	//Reset to the original matrix
	shader->Model.PopMatrix();

	// ---------------------------------
	// |||||||| DAMAGE INDICATOR |||||||
	// ---------------------------------

	//Preserve the current matrix
	shader->Model.PushMatrix();

	//Move 0,0 to the direct center of the screen
	shader->Model.Translate(viewPortSize.x*.5f,viewPortSize.y*.5f,0);

	float dist = min(viewPortSize.x, viewPortSize.y)/2.0f * .75f;
	double simTime = Game()->Now();

	for(auto it = damagePoints.begin(); it != damagePoints.end();) {
		if(simTime > (*it).first) {
			//Erase if a Damage Indicator expires
			it = damagePoints.erase(it);
		} else {
			//Gather information about Damage Point
			double ptTime = (*it).first;
			float x = (*it).second.x;
			float y = (*it).second.y;
			float damageAngle = 180 / M_PI * atan2(y - py, x - px);

			//Save Center of Screen Location
			shader->Model.PushMatrix();

			//Rotate to Angle of Damage relative to Player
			shader->Model.Rotate(playerAngle - damageAngle, vec3(0, 0, 1));

			//Translate Outwards to Damage Indicator Circle
			shader->Model.Translate(0,-dist,0);

			//Offset by the size of the arrow so it draws from the center
			shader->Model.Translate(-DAMAGE_INDICATOR_SIZE*.5f,-DAMAGE_INDICATOR_SIZE*.5f,0);

			//Apply all the model changes we've made
			shader->Model.Apply();

			//Set Color Intensity based on Time on Screen
			damageIndicator.SetColor(vec4(7,.2,.2,(.66f * (ptTime - simTime) / DAMAGE_INDICATOR_TIME)));

			//Draw Damage Indicator
			damageIndicator.Draw(shader);

			//Reset to Center of Screen
			shader->Model.PopMatrix();

			//Iterate to Next Indicator
			it++;
		}
	}

	//Reset to the original matrix
	shader->Model.PopMatrix();
}

void HUD::MarkDamage(vec2 source) {
	damagePoints.push_back(pair<double, vec2>(Game()->Now() + DAMAGE_INDICATOR_TIME, source));
}