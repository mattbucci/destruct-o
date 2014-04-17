#include "stdafx.h"
#include "MenuFrame.h"
#include "GL2DProgram.h"
#include "GL3DProgram.h"

#include "Window.h"
#include "Button.h"
#include "Listbox.h"

#include "OS.h"


MenuFrame::MenuFrame(ShaderGroup * shaders)
: GameSystem(shaders)
{

	cout << "\t Finished base frame\n";
	//testSystem = NULL;
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
{

}

bool MenuFrame::Update(vector<InputEvent> inputEvents) {
	return true;
}


void MenuFrame::Draw(double width, double height)
{

	//Call the parent draw to draw interface
	GameSystem::Draw(width, height);
}

