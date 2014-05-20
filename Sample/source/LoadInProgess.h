
#pragma once

#include "stdafx.h"

class LoadInProgress {
	string label;
	int progressCount;
	int total;
	bool useProgressCount;
	//Protect the string
	mutex progressMutex;
public:
	LoadInProgress();
	//Get the string representing the current progress
	string GetCurrentProgress();
	//Indicate that you're loading/doing a thing
	void Update(const string & label);
	//Indicate taht you're loading/doing a thing
	//and that your progress is progress/total
	void Update(const string & label, int progressCount, int total);
};