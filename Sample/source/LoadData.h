
#pragma once

#include "stdafx.h"

class LoadData {
	struct loadhandle {
		void ** handleToLoad;
		uint64_t originalHandle;
	};
	vector<loadhandle> handlesToLoad;
	//matches an original handle to a created handle
	map<uint64_t,void*> handlesRebuilt;

	int handleSize;
public:
	LoadData() {
		handleSize = 0;
	}

	//Register a recreated object
	void RegisterLoadedHandle(uint64_t original, void * newHandle);
	//Register a pointer to a recreated object which has not been created yet
	void RegisterHandleToLoad(uint64_t original, void ** handleToLoad);

	//Retrieves a handle which has already been constructed 
	void * RetrieveConstructedHandle(uint64_t original);

	//Use the list of built handles and the
	//originalHandles to match created objects
	//to old pointers to created objects
	//and patch up the pointers to point to the new objects
	void FinishLoading();
};