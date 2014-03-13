
#pragma once

#include "Window.h"
#include "Button.h"
#include "FileDialog.h"

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
public:
	FileMenuWindow(Editor * designer);
	~FileMenuWindow();
};