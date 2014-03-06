
#pragma once

#include "stdafx.h"

class GObject;

class GameSave {
public:
	enum Status {
		INPROGRESS,
		FAILED,
		SUCCEEDED,
	};

	struct Progress {
		//The number of objects to save
		int progress;
		//The max value
		int maxValue;
		//the status of the save
		Status save;
		//The error to display to the user
		string error;
	};
	
	//Used by SObjects and VObjects during program start to register their types, always returns 0
	static int8_t RegisterType(string typeName, function<GObject*(void)> toRegister);

	//Start save/load process
	static void StartAsyncSave(string filename);
	static void StartAsyncLoad(string filename);

	//Get the progress of the last save/load started
	static Progress GetOperationProgress(void);
private:
	GameSave();

	//Runs the save/load process. Runs from a separate thread
	void Load(string filename);
	void Save(string filename);

	//The function used to call Save() Load(), is the entry point for the thread
	friend void ThreadEntry(string filename, bool doSave);
	
	//A function used to store the map of type-name-to-type-constructor on-demand
	static map<string,function<GObject*(void)>> & GetConstructorFunction();

	Progress currentOperation;
	//the singleton instance of the game save system
	static GameSave instance;
};