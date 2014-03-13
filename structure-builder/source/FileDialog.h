
#pragma once

#include "Window.h"

#include "Button.h"
#include "Listbox.h"
#include "Textbox.h"
#include "Label.h"

#include "GameEventSubscriber.h"

class FileDialog;

typedef function<void(FileDialog * clickedDialog, string filePath)> FileCallback;

class FileDialog : public Window {
	bool isShown;

	Listbox files;
	Button ok;
	Button cancel;
	Label pathLabel;
	Textbox pathName;

	string extension;

	//For save/load
	string currentPath;
	void updatePath();
	FileCallback onComplete;
	bool isSaving;
public:
	FileDialog(string extension);
	~FileDialog();

	//onComplete is fired when OK or Cancel is pressed. filePath is empty if cancel was pressed
	void ShowSaveDialog(string startPath, FileCallback onComplete);
	void ShowLoadDialog(string startPath, FileCallback onComplete);
};