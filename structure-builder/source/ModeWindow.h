
#pragma once

#include "Window.h"
#include "Button.h"

class Editor;

class ModeWindow : public Window{
	vector<Button> modes; 

	Editor * structureEditor;
public:
	//Update which mode is shown to be active
	void Update();

	ModeWindow(Editor * structureEditor);
	~ModeWindow();
};