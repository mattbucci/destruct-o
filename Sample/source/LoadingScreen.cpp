#include "stdafx.h"
#include "LoadingScreen.h"
#include "lodepng.h"
#include "GL2DProgram.h"
#include "Font.h"
#include "GLTexture.h"
#include "OS.h"
#include "VoxEngine.h"

//Fonts
Font * LoadingScreen::titleText;
Font * LoadingScreen::infoText;
GLTexture * LoadingScreen::texture;

//The time it takes for one complete turn of the spinner
static const float spinningTime = .75f;

LoadingScreen::LoadingScreen() : GL2DVertexGroup(GL_TRIANGLE_STRIP,4) {
	static bool setupFonts = false;
	//Setup fonts/texture once per program execution
	if (!setupFonts) {
		setupFonts = true;
		titleText = new Font("Interface/Fonts/times.ttf",150);
		infoText = new Font("Interface/Fonts/times.ttf",60);
		texture = new GLTexture("loadingspinner.png");
		texture->CacheTexture();
		if (!texture->IsCached()) {
			delete texture;
			texture = GLTexture::GenerateErrorTexture();
		}
	}

	//Draw the classic square
	svat(0,vec2(0,0));
	sxat(0,vec2(0,0));
	svat(1,vec2(0,200));
	sxat(1,vec2(0,1));
	svat(2,vec2(200,0));
	sxat(2,vec2(1,0));
	svat(3,vec2(200,200));	
	sxat(3,vec2(1,1));

	bigText = RasterizedText("DESTRUCT-O",titleText,vec4(1,1,1,1));
	littleText = RasterizedText("...",infoText,vec4(1,1,1,1));
}
LoadingScreen::~LoadingScreen() {
}

//Draw the loading screen
void LoadingScreen::Draw(int width, int height, GL2DProgram * shader) {

	//Enable sensible defaults
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shader->UseProgram();
	//Resize automatically
	shader->SetWidthHeight(width,height);
	shader->EnableTexture(true);
	//bind the spinner texture
	texture->Bind();
	//select a frame for the spinner (8 frames in the spinner)
	float frame = (int)((fmod(OS::Now(),spinningTime)/spinningTime) * 8);
	
	//draw that frame
	sxat(0,vec2((frame)/8,0));
	sxat(1,vec2((frame)/8,1));
	sxat(2,vec2((frame+1)/8,0));
	sxat(3,vec2((frame+1)/8,1));

	shader->Model.Reset();
	shader->Model.Apply();
	shader->Clipping.PushClippingRect(shader->Model,Rect(-1,-1,-1,-1));
	shader->SetColor(vec4(1,1,1,1));

	//Place the spinner in the center of the screen
	shader->Model.PushMatrix();
	shader->Model.Translate(width/2.0-100,height/2.0-100,0);
	shader->Model.Apply();
	GL2DVertexGroup::Draw(shader);
	shader->Model.PopMatrix();

	//Place the big text a bit down from the top
	shader->Model.PushMatrix();
	shader->Model.Translate(width/2.0-bigText.Size().x/2.0,height*.1,0);
	shader->Model.Apply();
	bigText.Draw(shader);
	shader->Model.PopMatrix();

	//Update rasterized text if progress text changed
	string progressText = VoxEngine::LoadProgress.GetCurrentProgress();
	littleText.SetText(progressText);

	//draw along bottom
	if (progressText.size() > 0) {
		shader->Model.PushMatrix();
		shader->Model.Translate(width/2.0-littleText.Size().x/2.0,height*.9-littleText.Size().y,0);
		shader->Model.Apply();
		littleText.Draw(shader);
		shader->Model.PopMatrix();
	}


	shader->Clipping.PopClippingRect();
}