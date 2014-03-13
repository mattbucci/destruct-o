#include "stdafx.h"
#include "FileMenuWindow.h"
#include "StructureDesigner.h"

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
	AddChild(&fileSelect);

	//Setup behavior of each button
	Subscribe<void(Button*)>(&Open.EventClicked,[this](Button * clicked) {

	});
}
FileMenuWindow::~FileMenuWindow() {

}