#include "stdafx.h"
#include "FileDialog.h"


FileDialog::FileDialog(string extension) {
	//Setup window size/position
	position = Rect(0,0,300,200);
	hPin = Control::CENTER;
	vPin = Control::CENTER;
	SetTitle("FileMenu");

	//Arrange text and buttons
	ok.hPin = Control::MAX;
	ok.vPin = Control::MAX;
	cancel.hPin = Control::MAX;
	cancel.vPin = Control::MAX;
}
FileDialog::~FileDialog() {

}

void FileDialog::ShowSaveDialog(string startPath, function<void(FileDialog * clickedDialog, string filePath)> onOK) {

}
void FileDialog::ShowLoadDialog(string startPath, function<void(FileDialog * clickedDialog, string filePath)> onOK) {

}
