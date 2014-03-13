#include "stdafx.h"
#include "dirent.h"
#include "FileDialog.h"


FileDialog::FileDialog(string extension) {
	//Setup window size/position
	position = Rect(0,0,500,350);
	hPin = Control::CENTER;
	vPin = Control::CENTER;
	SetTitle("FileMenu");

	//Arrange text and buttons
	ok.hPin = Control::MAX;
	ok.vPin = Control::MAX;
	cancel.hPin = Control::MAX;
	cancel.vPin = Control::MAX;

	ok.position = Rect(-10,-10,60,30);
	cancel.position = Rect(-80,-10,60,30);
	ok.SetText("OK");
	cancel.SetText("Cancel");

	//Setup file listbox
	files.position = Rect(20,50,460,240);

	pathLabel.vPin = Control::MAX;
	pathLabel.position = Rect(10,-15,200,200);
	pathLabel.SetText("Filename: ");

	//And the textbox path
	pathName.vPin = Control::MAX;
	pathName.position = Rect(90,-10,260,30);

	//Add controls
	AddChild(&ok);
	AddChild(&cancel);
	AddChild(&files);
	AddChild(&pathLabel);
	AddChild(&pathName);

	SetVisible(false);
	this->extension = extension;

	//Handle single clicking for setting filename
	Subscribe<void(Listbox*,int)>(&files.EventSelectionChanged,[this](Listbox * lb, int selectedEntry) {
		string name = lb->GetEntries()[selectedEntry];
		//Select file names
		if ((name.size() <= 2) || (name[0] != '[') || (name[name.size()-1] != ']')) {
			pathName.SetText(name);
		}
	});	

	//Handle double clicking on folders and such
	Subscribe<void(Listbox*,int)>(&files.EventSelectionDoubleClicked,[this](Listbox * lb, int selectedEntry) {
		string name = lb->GetEntries()[selectedEntry];
		//Go into directories
		if ((name.size() > 2) && (name[0] == '[') && (name[name.size()-1] == ']')) {
			currentPath += "/" + name.substr(1,name.size()-2);
			updatePath();
		}
		else {
			//Mark this operation complete
			//Select the file double clicked on
			onComplete(this,currentPath + "/" + name);
			SetVisible(false);
		}
	});

	//Handle buttons
	Subscribe<void(Button*)>(&ok.EventClicked,[this](Button * b) {
		//Mark this operation complete
		//Select the text entered
		onComplete(this,currentPath + "/" + pathName.GetText());
		SetVisible(false);
	});
	Subscribe<void(Button*)>(&cancel.EventClicked,[this](Button * b) {
		//Mark this operation complete
		//Send empty text since they clicked cancel
		onComplete(this,"");
		SetVisible(false);
	});


	//this->extension = ".sln";
	//ShowSaveDialog(".",[](FileDialog * c, string s){});
}
FileDialog::~FileDialog() {

}

//string currentPath;
void FileDialog::updatePath() {
	list<string> fileNames;

	DIR *dir;
	struct dirent *ent;
	//Read and compile the lua snippets
	if ((dir = opendir(currentPath.c_str())) != NULL) {
		while ((ent = readdir (dir)) != NULL) {
			struct stat sb;
			string fname = ent->d_name;
			//Skip the current folder
			if (fname == ".")
				continue;
			//Check if the path is a folder
			if ((stat((currentPath + "/" + fname).c_str(), &sb) == 0) && S_ISDIR(sb.st_mode))
				fileNames.push_front(string("[") + fname + "]");
			else {
				//Check that the filename matches the extension
				//note extension is case sensitive right now
				if ((fname.size() >= extension.size()) 
					&& (fname.substr(fname.size()-extension.size(),extension.size()) == extension))
					fileNames.push_back(fname);
			}
				
		}
		closedir (dir);
	}

	files.SetEntries(fileNames);
}

void FileDialog::ShowSaveDialog(string startPath, FileCallback onComplete) {
	//Clear the settings and show default files
	currentPath = startPath;
	this->onComplete = onComplete;
	pathName.SetText("");
	updatePath();
	//Update button text to reflect saving
	ok.SetText("Save");
	SetVisible(true);
	//Mark that you're saving
	isSaving = true;
}
void FileDialog::ShowLoadDialog(string startPath, FileCallback onComplete) {
	//Clear the settings and show default files
	currentPath = startPath;
	this->onComplete = onComplete;
	pathName.SetText("");
	updatePath();
	//Update button text to reflect loading
	ok.SetText("Open");
	SetVisible(true);
	//Mark that you're opening
	isSaving = false;
}
