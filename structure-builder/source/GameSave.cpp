#include "stdafx.h"
#include "GameSave.h"
/*
GameSave GameSave::instance;

//The function used to call Save() Load(), is the entry point for the thread
static void ThreadEntry(string filename, bool doSave) {
	//Run the save operation 
	if (doSave)
		GameSave::instance.Save(filename);
	else
		GameSave::instance.Load(filename);
}

int8_t GameSave::RegisterType(string typeName, function<GObject*(void)> toRegister) {
	//Check to make sure the type name doesn't already exist in debug
	_ASSERTE(GetConstructorFunction().find(typeName) == GetConstructorFunction().end());
	//Save the type constructor under the specified type name
	GetConstructorFunction()[typeName] = toRegister;

	return 0;
}

//Start save/load process
void GameSave::StartAsyncSave(string filename) {
	//No starting an operation if you're already in an operation
	_ASSERTE((instance.currentOperation.progress == FAILED) ||
		(instance.currentOperation.progress == SUCCEEDED));

	//Mark that you're starting an operation
	instance.currentOperation.progress = INPROGRESS;

	//Start the thread the operation will run on
	thread t(ThreadEntry,filename,true);
	t.detach();
}
void GameSave::StartAsyncLoad(string filename) {
	//No starting an operation if you're already in an operation
	_ASSERTE((instance.currentOperation.progress == FAILED) ||
		(instance.currentOperation.progress == SUCCEEDED));

	//Mark that you're starting an operation
	instance.currentOperation.progress = INPROGRESS;

	//Start the thread the operation will run on

	thread t(ThreadEntry,filename,false);
	t.detach();
}

//Get the progress of the last save/load started
GameSave::Progress GameSave::GetOperationProgress(void) {
	return instance.currentOperation;
}

GameSave::GameSave() {
	//Just pretend you completed an operation
	currentOperation.progress = SUCCEEDED;
}

//Runs the save/load process. Runs from a separate thread
void GameSave::Load(string filename) {

}
void GameSave::Save(string filename) {

}

	
//A function used to store the map of type-name-to-type-constructor on-demand
map<string,function<GObject*(void)>> & GameSave::GetConstructorFunction() {
	//Declared in a function like this it's automatically allocated on demand
	static map<string,function<GObject*(void)>> allocateOnDemand;
	return allocateOnDemand;
}
*/