
#pragma once

#include "Window.h"
#include "Button.h"
#include "FileDialog.h"
#include "ErrorWindow.h"

class Editor;
class Structure;

class FileMenuWindow : public Window {
	Structure * beingEdited;
	Editor * designer;
	//Interface elements
	Button Open;
	Button Close;
	Button Save;
	Button SaveAs;
	//Button/path
	bool fileOpen;
	string currentFilePath;
	//Do a save-as (called by save if a normal save fails)
	void DoSaveAs();
public:
	FileMenuWindow(Editor * designer);
	~FileMenuWindow();
};