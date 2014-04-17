#include "stdafx.h"
#include "HUD.h"
#include "GL2DProgram.h"
#include "VoxEngine.h"
#include "BaseFrame.h"

HUD::HUD() :
	//Use the tint to tint the white arrow mostly red
	arrowExample(Rect(0,0,50,50),"hud/arrow.png",vec4(7,.2,.2,1)),
	//Again use tint to make the white dot bright red (or green or orange or anything)
	//Note you can change the tint on the fly
	minimapDotExample(Rect(0,0,20,20),"hud/dot.png",vec4(1,0,0,1)),
	minimapBackgroundExample(Rect(0,0,200,200),"hud/minimap.png") {

}
//Since the hud is purely visual
//there is no point in updating it at 100hz
//it might as well update as its drawn
void HUD::DrawAndUpdate(GL2DProgram * shader, vec2 viewPortSize) {
	//First do the arrow
	//Preserve the current matrix
	shader->Model.PushMatrix();
	//Move 0,0 to the direct center of the screen
	shader->Model.Translate(viewPortSize.x*.5f,viewPortSize.y*.5f,0);
	//Rotate some degree depending on which angle you want the arrow to show from
	//In this case we've chosen a trick using the time
	//to make the arrow rotate for no reason
	shader->Model.Rotate(fmod(Game()->Now(),10)/10*360,vec3(0,0,1));
	//Translate outwards
	//Puts 0 degrees directly up
	shader->Model.Translate(0,-200,0);
	//Offset by the size of the arrow so it draws from the center
	shader->Model.Translate(-25,-25,0);

	//Apply all the model changes we've made
	shader->Model.Apply();
	//draw the arrow
	arrowExample.Draw(shader);

	//Reset to the original matrix
	shader->Model.PopMatrix();


	


	//Next do the dot
	//Preserve the current matrix
	shader->Model.PushMatrix();

	//Translate to the edge of the minimap
	shader->Model.Translate(viewPortSize.x*.05f,viewPortSize.y*.6f,0);

	shader->Model.Apply();
	//draw background
	minimapBackgroundExample.Draw(shader);

	//Translate to a location inside of the minimap
	shader->Model.Translate(viewPortSize.x*.07f,viewPortSize.y*.04f,0);

	//Apply all the model changes we've made
	shader->Model.Apply();

	//Make the dot pulse gently
	float period = sin(fmod(Game()->Now(),3.141592));
	minimapDotExample.SetColor(vec4(1,0,0,.5f+.5f*period));

	//Draw the dot
	minimapDotExample.Draw(shader);

	//Reset to the original amtrix
	shader->Model.PopMatrix();
}