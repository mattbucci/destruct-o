#include "stdafx.h"
#include "HUD.h"
#include "GL2DProgram.h"
#include "VoxEngine.h"
#include "BaseFrame.h"

#define MINIMAP_SIZE 200
#define MINIMAP_DOT_SIZE 20

#define DAMAGE_INDICATOR_TIME 1

HUD::HUD() :
	//Use the tint to tint the white arrow mostly red
	damageIndicator(Rect(0,0,100,100),"hud/arrow.png",vec4(7,.2,.2,.66)),
	//Again use tint to make the white dot bright red (or green or orange or anything)
	//Note you can change the tint on the fly
	minimapDot(Rect(0,0,MINIMAP_DOT_SIZE,MINIMAP_DOT_SIZE),"hud/dot.png",vec4(1,0,0,1)),
	minimapBackground(Rect(0,0,MINIMAP_SIZE,MINIMAP_SIZE),"hud/minimap.png", vec4(1, 1, 1, .6)) {
}
//Since the hud is purely visual
//there is no point in updating it at 100hz
//it might as well update as its drawn
void HUD::DrawAndUpdate(GL2DProgram * shader, vec2 viewPortSize) {

	// ---------------------------------
	// |||||||||||| MINIMAP ||||||||||||
	// ---------------------------------

	//Preserve the current matrix
	shader->Model.PushMatrix();

	//Translate to the edge of the minimap
	shader->Model.Translate(0.0f, viewPortSize.y - MINIMAP_SIZE - 0.0f, 0);

	//Apply Matrix Transformations
	shader->Model.Apply();

	//Draw Background
	minimapBackground.Draw(shader);

	//Translate to Center of Minimap
	shader->Model.Translate(MINIMAP_SIZE * .5f - MINIMAP_DOT_SIZE * .5f, MINIMAP_SIZE * .5f - MINIMAP_DOT_SIZE * .5f,0);

	//Apply all the model changes we've made
	shader->Model.Apply();

	minimapDot.SetColor(vec4(0, 0, 0, 1));

	//Draw the dot
	minimapDot.Draw(shader);

	//Make the dot pulse gently
	float period = sin(fmod(Game()->Now(),3.141592));
	minimapDot.SetColor(vec4(1,0,0,.5f+.5f*period));

	for(int i = 0; i < 10; i++) {
		int x = rand() % (MINIMAP_SIZE-20) - (MINIMAP_SIZE-20) * .5f;
		int y = rand() % (MINIMAP_SIZE-20) - (MINIMAP_SIZE-20) * .5f;

		//Translate to Center of Minimap
		shader->Model.PushMatrix();
		shader->Model.Translate(x, y, 0);
		shader->Model.Apply();

		minimapDot.Draw(shader);

		shader->Model.PopMatrix();
	}

	//Reset to the original amtrix
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
			//Don't Erase for Now... Just put it somewhere else
			//it = damagePoints.erase(it);

			//----- Ooo Fancy -----
			(*it) = pair<double, vec2>(Game()->Now() + DAMAGE_INDICATOR_TIME + (rand() % 100)/100.0f, vec2(rand() % 360 - 360/2.0f, rand() % 360 - 360/2.0f));
			it++;
			//---------------------
		} else {
			double ptTime = (*it).first;
			shader->Model.PushMatrix();

			//Rotate some degree depending on which angle you want the arrow to show from
			//In this case we've chosen a trick using the time
			//to make the arrow rotate for no reason
			shader->Model.Rotate((*it).second.x,vec3(0,0,1));

			//Translate outwards
			//Puts 0 degrees directly up
			shader->Model.Translate(0,-dist,0);

			//Offset by the size of the arrow so it draws from the center
			shader->Model.Translate(-50,-50,0);

			//Apply all the model changes we've made
			shader->Model.Apply();

			damageIndicator.SetColor(vec4(7,.2,.2,(.66f * (ptTime - simTime) / DAMAGE_INDICATOR_TIME)));

			//Draw Damage Indicator
			damageIndicator.Draw(shader);

			shader->Model.PopMatrix();

			it++;
		}
	}

	//Reset to the original matrix
	shader->Model.PopMatrix();
}