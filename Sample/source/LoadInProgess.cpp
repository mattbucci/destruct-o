#include "stdafx.h"
#include "LoadInProgess.h"
#include "Utilities.h"

LoadInProgress::LoadInProgress() {
	useProgressCount = false;
}
//Get the string representing the current progress
string LoadInProgress::GetCurrentProgress() {
	lock_guard<mutex> guard(progressMutex);
	if (useProgressCount) 
		return label + " (" + Utilities::toString(progressCount) + "/" + Utilities::toString(total) + ")";
	return label;
}
//Indicate that you're loading/doing a thing
void LoadInProgress::Update(const string & label) {
	lock_guard<mutex> guard(progressMutex);
	if (this->label != label)
		this->label = label;
	useProgressCount = false;
}
//Indicate taht you're loading/doing a thing
//and that your progress is progress/total
void LoadInProgress::Update(const string & label, int progressCount, int total) {
	lock_guard<mutex> guard(progressMutex);
	if (this->label != label)
		this->label = label;
	this->progressCount = progressCount;
	this->total = total;
	useProgressCount = true;
}