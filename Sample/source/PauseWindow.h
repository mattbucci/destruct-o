#pragma once

#include "Window.h"
#include "Button.h"


class BaseFrame;

class PauseWindow : public Window
{
	int w, h;
protected:

	//Main Menu Elements
	Window menuRect;
	Button menuSave;
	Button menuLoad;
	Button menuOptions;
	Button menuSaveAndQuit;
	Button menuExit;

	//Options Menu Elements
	Window optsRect;
	Button optsClose;

	typedef pair<string,float> floatOption;
	void AddSliderOption(int sliderNumber, string sliderName, vector<floatOption> options, float * appliesTo);

public:
	PauseWindow();
	~PauseWindow(void);

	bool toggle();

	virtual void Draw(GL2DProgram * shaders) override;

private:
	void showOptsMenu();
	void hideOptsMenu();
	void resizeElement();
};

