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

	//testSystem = NULL;
	Window * window = new Window(Rect(0, 0, 200, 400), "On Bottom");
	window->hPin = Control::CENTER;
	window->vPin = Control::MAX;
	window->SetVisible(false);
	Controls.AddWindow(window);
	//Give it a list box with a bunch of random crap
	vector<string> randomCrap;
	randomCrap.push_back("Monkey");
	randomCrap.push_back("Walrus");
	randomCrap.push_back("Cheeto");
	randomCrap.push_back("Spaghetii");
	randomCrap.push_back("Canada");
	randomCrap.push_back("Banana");
	randomCrap.push_back("One");
	randomCrap.push_back("Seventeen");
	randomCrap.push_back("Lamp");
	randomCrap.push_back("Voxel");
	randomCrap.push_back("Game");
	randomCrap.push_back("Walrus");
	randomCrap.push_back("Still Walrus");
	randomCrap.push_back("Cloud");
	Listbox * list = new Listbox();
	list->position = Rect(0, 0, 180, 150);
	list->hPin = list->vPin = Control::CENTER;
	list->SetEntries(randomCrap);
	//When the user selects something, change the title to the 
	//selected item
	window->AddControl(list);
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



}

bool MenuFrame::Update(vector<InputEvent> inputEvents) {
	return true;
}


void MenuFrame::Draw(double width, double height)
{

	//Call the parent draw to draw interface
	GameSystem::Draw(width, height);
}

