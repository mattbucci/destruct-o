#include "stdafx.h"
#include "MenuFrame.h"
#include "GL2DProgram.h"
#include "GL3DProgram.h"
#include "Frames.h"

#include "OS.h"


MenuFrame::MenuFrame(ShaderGroup * shaders)
: GameSystem(shaders),
  logo(Rect(0, 0, 648, 150), "menu/logo.png", vec4(1,1,1,1)),
  background(Rect(0, 0, 800, 600),"menu/background.png", vec4(1,1,1,1))
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
		Frames::SetSystem(Frames::FRAME_GAME);
	});

	Subscribe<void(Button*)>(&loadButton.EventClicked, [this](Button * b) {
	});

	Subscribe<void(Button*)>(&optionsButton.EventClicked, [this](Button * b) {
	});
}

MenuFrame::~MenuFrame()
{

}



//synchronously saves the game
bool MenuFrame::Save(string saveFile) {

	return true;
}
//synchronously loads the game over any existing data
bool MenuFrame::Load(string saveFile) {

	return true;
}


void MenuFrame::OnFrameFocus() {
}

void MenuFrame::Build()
{    //Build the sample dialog 
	//Build a window that says "On Top"

	isReady = true;

}

bool MenuFrame::Update(vector<InputEvent> inputEvents) {
	//Pass Update to GameSystem to Handle Control Input
	return GameSystem::Update(inputEvents);
}


void MenuFrame::Draw(double width, double height)
{
	GL2DProgram * shaders2d = SetWidthHeight(width, height);

	shaders2d->Model.Reset();
	shaders2d->Model.Apply();
	background.Draw(shaders2d);
	shaders2d->Model.Translate(width/2.0f - 648/2.0f, 80, 0);
	shaders2d->Model.Apply();
	logo.Draw(shaders2d);
	shaders2d->Model.Reset();
	shaders2d->Model.Apply();

	//Call the parent draw to draw interface
	GameSystem::Draw(width, height);
}

