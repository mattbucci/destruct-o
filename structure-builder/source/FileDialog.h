
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
	//If saveDialog is true, is a save dialog
	//otherwise is a load dialog
	//startPath should be "." for the current directory
	FileDialog(bool saveDialog, string extension, string startPath, FileCallback onComplete);
	~FileDialog();

	static void ShowFileDialog(bool saveDialog, string extension, string startPath, FileCallback onComplete);
};