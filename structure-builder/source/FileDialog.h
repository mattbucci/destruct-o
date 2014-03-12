
#pragma once

#include "Window.h"

#include "Button.h"
#include "Listbox.h"
#include "Textbox.h"

class FileDialog : public Window{
	bool isShown;

	Listbox files;
	Button ok;
	Button cancel;
	Textbox pathName;

	string extension;
public:
	FileDialog(string extension);
	~FileDialog();

	void ShowSaveDialog(string startPath, function<void(FileDialog * clickedDialog, string filePath)> onOK);
	void ShowLoadDialog(string startPath, function<void(FileDialog * clickedDialog, string filePath)> onOK);
};