#include "stdafx.h"
#include "FileMenuWindow.h"
#include "StructureDesigner.h"
#include "Frames.h"
#include "GameSystem.h"
#include "Structure.h"
#include "Editor.h"

FileMenuWindow::FileMenuWindow(Editor * designer) : fileSelect(".struct") {
	//Setup file menu in the upper left
	position = Rect(20,20,350,50);

	//Position buttons all next to each other
	Open.vPin = CENTER;
	Close.vPin = CENTER;
	Save.vPin = CENTER;
	SaveAs.vPin = CENTER;
	Open.position = Rect(20,0,70,30);
	Close.position = Rect(100,0,70,30);
	Save.position = Rect(180,0,70,30);
	SaveAs.position = Rect(260,0,70,30);

	//Set text
	Open.SetText("Open");
	Close.SetText("Close");
	Save.SetText("Save");
	SaveAs.SetText("Save As");

	//Add all the controls to the filemenu
	AddChild(&Open);
	AddChild(&Close);
	AddChild(&Save);
	AddChild(&SaveAs);
	//Add window to the window system
	//CurrentSystem->Controls.AddWindow(&fileSelect);

	//Set defaults
	fileOpen = false;

	//Setup behavior of each button
	Subscribe<void(Button*)>(&Open.EventClicked,[this](Button * clicked) {
		//Prevent them from opening something if something else is open
		if (fileOpen) {
			ShowError("You must close the current file first.");
			return;
		}

		fileSelect.ShowLoadDialog(".",[this](FileDialog * clickedDialog, string filePath) {
			//Make sure they made a selection
			if (filePath.size() <= 0)
				return;

			//Cut the ".be.struct" off the name
			filePath = filePath.substr(0,filePath.size()-10);

			//Open that file
			Structure * toLoad = Structure::LoadStructure(filePath);
			beingEdited = toLoad;
			if (toLoad == NULL) {
				ShowError("Failed to open that structure file.");
				return;
			}
			//File has been opened
			fileOpen = true;
			this->designer->EditStructure(toLoad);
			this->designer->EnableEditor(true);
		});
	});
	Subscribe<void(Button*)>(&Close.EventClicked,[this](Button * clicked) {
		//Prevent them from opening something if something else is open
		if (!fileOpen) {
			ShowError("Nothing to close.");
			return;
		}
		//Just disable the designer and mark the file as closed
		this->designer->EnableEditor(false);
		fileOpen = false;
		currentFilePath = "";
	});
	Subscribe<void(Button*)>(&Save.EventClicked,[this](Button * clicked) {
		//Prevent them from opening something if something else is open
		if (!fileOpen) {
			ShowError("Nothing to close.");
			return;
		}
		//Check there is a file path to save to
		if (currentFilePath.size() >= 0) 
			//Do the save
			beingEdited->SaveStructure(currentFilePath);
		else
			//Force a save as for the user to set a path
			DoSaveAs();
	});
}

void FileMenuWindow::DoSaveAs() {
	if (!fileOpen) {
		ShowError("Nothing to save.");
		return;
	}

	//Open the save dialog
	fileSelect.ShowSaveDialog(".",[this](FileDialog * clickedDialog, string filePath) {
		//Make sure they made a selection
		if (filePath.size() <= 0)
			return;
		//If it has the extension, cut it off
		if ((filePath.size() >= 7) && (filePath.substr(filePath.size()-7,7) == ".struct")) 
			filePath = filePath.substr(0,filePath.size()-7);
		//Now save the file
		beingEdited->SaveStructure(filePath);
		//Save the path for fast saving
		currentFilePath = filePath;
	});
}

void FileMenuWindow::ShowError(string error) {

}

FileMenuWindow::~FileMenuWindow() {

}