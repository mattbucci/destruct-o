
#pragma once

#include "Window.h"
#include "Button.h"
#include "FileDialog.h"
#include "ErrorWindow.h"

class Editor;

class FileMenuWindow : public Window {
	Editor * designer;
	//Interface elements
	FileDialog fileSelect;
	Button Open;
	Button Close;
	Button Save;
	Button SaveAs;
	//Button/path
	bool fileOpen;
	string currentFilePath;
	//Show an error
	void ShowError(string error);
	//Do a save-as (called by save if a normal save fails)
	void SaveAs();
public:
	FileMenuWindow(Editor * designer);
	~FileMenuWindow();
};