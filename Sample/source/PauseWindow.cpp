#include "stdafx.h"

#include "PauseWindow.h"
#include "BaseFrame.h"
#include "Slider.h"
#include "Frames.h"
#include "VoxEngine.h"

typedef pair<string,int> intOption;



void PauseWindow::AddSliderOption(int sliderNumber, string sliderName, vector<floatOption> options, float * appliesTo) {
	Label * sliderLabel = new Label();
	Slider<float> * slider = new Slider<float>();

	//Option View Distance
	sliderLabel->vPin = slider->vPin = Control::MIN;
	sliderLabel->hPin = slider->hPin = Control::CENTER;
	sliderLabel->position = Rect(0, 50 + 65 * sliderNumber, 300, 20);
	slider->SetPosition(Rect(0, 80 + 65 * sliderNumber, 300, 25));
	sliderLabel->SetText(sliderName);
	slider->SetValue(appliesTo);
	optsRect.AddControl(sliderLabel);
	optsRect.AddControl(slider);
    

	slider->SetElements(options);
}

PauseWindow::PauseWindow()
{
	w = 0, h = 0;

	//Set up Self
	position = Rect(0, 20, 800, 580);
	color = vec4(0.33f, 0.33f, 0.33f, 0.75f);
	SetVisible(true);

	//Set up Main Menu
	menuRect.position = Rect(0, 0, 320, 420);
	menuRect.vPin = menuRect.hPin = Control::CENTER;
	menuRect.SetTitle("Menu");
	menuRect.color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	//Set up Main Menu Children
	menuSave.vPin = menuLoad.vPin = menuOptions.vPin = menuSaveAndQuit.vPin = menuExit.vPin = Control::MIN;
	menuSave.hPin = menuLoad.hPin = menuOptions.hPin = menuSaveAndQuit.hPin = menuExit.hPin = Control::CENTER;
	menuSave.position =			Rect(0, 50 + 50 * 0, 300, 40);
	menuLoad.position =			Rect(0, 50 + 50 * 1, 300, 40);
	menuOptions.position =		Rect(0, 50 + 50 * 2, 300, 40);
	menuSaveAndQuit.position =	Rect(0, 50 + 50 * 3, 300, 40);
	menuExit.position =			Rect(0, 370, 300, 40);
	menuSave.SetText("Save");
	menuLoad.SetText("Load");
	menuOptions.SetText("Options");
	menuSaveAndQuit.SetText("Save & Quit");
	menuExit.SetText("Back to Game");

	//Add Controls
	AddChild(&menuRect);
	menuRect.AddControl(&menuSave);
	menuRect.AddControl(&menuLoad);
	menuRect.AddControl(&menuOptions);
	menuRect.AddControl(&menuSaveAndQuit);
	menuRect.AddControl(&menuExit);
	menuRect.SetVisible(true);

	//Subscribe Main Menu Buttons to Actions
	Subscribe<void(Button*)>(&menuSave.EventClicked, [this](Button * b) {
		VoxEngine::SetAsyncTask(new AsyncTask([](){Game()->Save();}));
		Frames::SetSystem(Frames::FRAME_GAME);
	});
	Subscribe<void(Button*)>(&menuLoad.EventClicked, [this](Button * b) {
		VoxEngine::SetAsyncTask(new AsyncTask([](){Game()->Load("Default_Save");}));
		Frames::SetSystem(Frames::FRAME_GAME);
	});
	Subscribe<void(Button*)>(&menuOptions.EventClicked, [this](Button * b) {
		this->showOptsMenu();
	});
	Subscribe<void(Button*)>(&menuSaveAndQuit.EventClicked, [this](Button * b) {
		cout << "\'Save & Quit\' Button Clicked" << endl;
		VoxEngine::SetAsyncTask(new AsyncTask([](){Game()->Save();}));
		
		Frames::SetSystem(Frames::FRAME_MAINMENU);
	});
	Subscribe<void(Button*)>(&menuExit.EventClicked, [this](Button * b) {
		Frames::SetSystem(Frames::FRAME_GAME);
	});

	//Set up Options Menu
	optsRect.position = Rect(0, 0, 320, 420);
	optsRect.vPin = optsRect.hPin = Control::CENTER;
	optsRect.SetTitle("Options");
	optsRect.color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	optsRect.SetVisible(false);
	AddChild(&optsRect);

	//Options Back Button
	optsClose.vPin = Control::MIN;
	optsClose.hPin = Control::CENTER;
	optsClose.position = Rect(0, 370, 300, 40);
	optsClose.SetText("Back");
	optsRect.AddControl(&optsClose);
	Subscribe<void(Button*)>(&optsClose.EventClicked, [this](Button * b) {
		this->hideOptsMenu();
	});

	//Option HUD Transparency
	floatOption HUDTransOpts[] = {
		floatOption("0%", .0f),
		floatOption("10%", .1f),
		floatOption("20%", .2f),
		floatOption("30%", .3f),
		floatOption("40%", .4f),
		floatOption("50%", .5f),
		floatOption("60%", .6f),
		floatOption("70%", .7f),
		floatOption("80%", .8f),
		floatOption("90%", .9f),
		floatOption("100%", 1.0f)
	};
	vector<floatOption> HUDTrans(HUDTransOpts, HUDTransOpts + sizeof(HUDTransOpts) / sizeof(HUDTransOpts[0]));
	AddSliderOption(0,"HUD Transparency",HUDTrans,&VoxEngine::SavedDeviceData.GameOptions.HUDTransparency);
    
    // View distance options
    floatOption viewDistanceOptions[] =
    {
        floatOption("TI-84", 0.00f),
        floatOption("Mobile", 0.25f),
        floatOption("Laptop", 0.5f),
        floatOption("Desktop", 0.75f),
        floatOption("$oopah Us3r", 1.0f),
    };
    vector<floatOption> viewDistance(viewDistanceOptions, viewDistanceOptions + (sizeof(viewDistanceOptions) / sizeof(viewDistanceOptions[0])));
	AddSliderOption(1,"View Distance",viewDistance,&VoxEngine::SavedDeviceData.GameOptions.ViewDistance);

    // Physics
    floatOption basicOptions[] =
    {
        floatOption("Very Low", 0.00f),
        floatOption("Low", 0.25f),
        floatOption("Medium", 0.5f),
        floatOption("High", 0.75f),
        floatOption("VeryHigh", 1.0f),
    };
	vector<floatOption> basicOptionsVector(basicOptions, basicOptions + (sizeof(basicOptions) / sizeof(basicOptions[0])));
	AddSliderOption(2,"Physics",basicOptionsVector,&VoxEngine::SavedDeviceData.GameOptions.PhysicsAccuracy);
	//Particles
	AddSliderOption(3,"Particles",basicOptionsVector,&VoxEngine::SavedDeviceData.GameOptions.ParticleQuality);

#ifndef __MOBILE__
    // jumping
    floatOption jumpOptions[] =
    {
        floatOption("Off", 0.00),
        floatOption("On", 2.00),
    };
	vector<floatOption> jumpOptionsVector(jumpOptions, jumpOptions + (sizeof(jumpOptions) / sizeof(jumpOptions[0])));
	AddSliderOption(4,"Jumping",jumpOptionsVector,&VoxEngine::SavedDeviceData.GameOptions.Autojump);
#endif

}


PauseWindow::~PauseWindow(void)
{
}

bool PauseWindow::toggle() {
	if(menuRect.GetVisible()) {
		SetVisible(false);
		menuRect.SetVisible(false);
		return false;
	} else {
		SetVisible(true);
		menuRect.SetVisible(true);
		return true;
	}
}

void PauseWindow::Draw(GL2DProgram * shaders) {
	if(w != (int)getParent()->position.Width || h != (int)getParent()->position.Height) {
		w = (int)getParent()->position.Width;
		h = (int)getParent()->position.Height;
		resizeElement();
	}
	Window::Draw(shaders);
}

void PauseWindow::showOptsMenu() {
	optsRect.SetVisible(true);
}

void PauseWindow::hideOptsMenu() {
	optsRect.SetVisible(false);
}

void PauseWindow::resizeElement() {
	position = Rect(0, 20, (float)w, (float)h-20);
}