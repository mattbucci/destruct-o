#include "stdafx.h"
#include "MenuFrame.h"
#include "GL2DProgram.h"
#include "GL3DProgram.h"
#include "Frames.h"
#include "BaseFrame.h"
#include "OS.h"


MenuFrame::MenuFrame(ShaderGroup * shaders)
    : GameSystem(shaders), background(Rect(0, 0, 800, 600),"menu/background.png", vec4(1,1,1,1)), logo(Rect(0, 0, 648, 150), "menu/logo.png", vec4(1,1,1,1))
{
	buttonWindow.hPin = Control::CENTER;
	buttonWindow.vPin = Control::MAX;
	buttonWindow.position = Rect(0, -50, 400, 300);
	Controls.AddWindow(&buttonWindow);

	playButton.hPin = Control::CENTER;
	playButton.vPin = Control::MIN;
	playButton.position = Rect(0, 10 + 96.66f*0, 380, 86.66f);
	playButton.SetText("Play");
	buttonWindow.AddControl(&playButton);

	loadButton.hPin = Control::CENTER;
	loadButton.vPin = Control::MIN;
	loadButton.position = Rect(0, 10 + 96.66f*1, 380, 86.66f);
	loadButton.SetText("Load");
	buttonWindow.AddControl(&loadButton);

	optionsButton.hPin = Control::CENTER;
	optionsButton.vPin = Control::MIN;
	optionsButton.position = Rect(0, 10 + 96.66f*2, 380, 86.66f);
	optionsButton.SetText("Options");
	buttonWindow.AddControl(&optionsButton);

	//Subscribe Main Menu Buttons to Actions
	Subscribe<void(Button*)>(&playButton.EventClicked, [this](Button * b) {
		//Tell the engine to load a new game
		VoxEngine::SetAsyncTask(new AsyncTask([]() {Game()->NewWorld();}));
		Frames::SetSystem(Frames::FRAME_GAME);
	});

	Subscribe<void(Button*)>(&loadButton.EventClicked, [this](Button * b) {
		//Tell the engine to load the only save
		//if the only save doesn't exist, something terrible happens
		VoxEngine::SetAsyncTask(new AsyncTask([]() {Game()->Load("Default_Save");}));
		Frames::SetSystem(Frames::FRAME_GAME);
		
	});

	Subscribe<void(Button*)>(&optionsButton.EventClicked, [this](Button * b) {
	});
}

MenuFrame::~MenuFrame()
{

}



void MenuFrame::OnFrameFocus() {
		//Tell the engine to load a new game
		//VoxEngine::SetAsyncTask(new AsyncTask([]() {Game()->NewWorld();}));
		//Frames::SetSystem(Frames::FRAME_GAME);
}



bool MenuFrame::Update(vector<InputEvent> inputEvents) {
	//Pass Update to GameSystem to Handle Control Input
	return GameSystem::Update(inputEvents);
}


void MenuFrame::Draw(double width, double height)
{
	float w = (float)width;
	float h = (float)height;
	//retrieve the texture
	GLTexture * texture = Textures.GetTexture<GLTexture>("menu/background.png");
	//Determine which dimension are constraints
	float widthRatio = w/texture->GetWidth();
	float heightRatio = h/texture->GetHeight();
	float aspectRatio = texture->GetWidth()/texture->GetHeight();
	float widthScale, heightScale;
	if (widthRatio > heightRatio) {
		//Stretch width to fit and size height appropriately
		widthScale = widthRatio;
		heightScale = widthScale/aspectRatio;
	}
	else {
		//stretch height to fit and size width appropriately
		heightScale = heightRatio;
		widthScale = heightScale*aspectRatio;
	}
	//Resize the image
	float imageWidth = widthScale * texture->GetWidth();
	float imageHeight = heightScale * texture->GetHeight();
	//Center the image
	Rect imageRect(w/2.0f-imageWidth/2.0f,h/2.0f-imageHeight/2.0f,imageWidth,imageHeight);
	background.SetRect(imageRect);

	GL2DProgram * shaders2d = SetWidthHeight((float)width, (float)height);

	shaders2d->Model.Reset();
	shaders2d->Model.Apply();
	background.Draw(shaders2d);
	shaders2d->Model.Translate((float)(width/2.0f - 648/2.0f), 80, 0);
	shaders2d->Model.Apply();
	logo.Draw(shaders2d);
	shaders2d->Model.Reset();
	shaders2d->Model.Apply();

	//Call the parent draw to draw interface
	GameSystem::Draw(width, height);
}
