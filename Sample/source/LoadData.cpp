#include "stdafx.h"
#include "LoadData.h"


//Register a recreated object
void LoadData::RegisterLoadedHandle(uint64_t original, void * newHandle) {
	//If the handle is being rebuilt twice, we have a problem
	_ASSERTE(handlesRebuilt.find(original) == handlesRebuilt.end());
	handleSize++;


	handlesRebuilt[original] = newHandle;
		if (handlesRebuilt.size() != handleSize) {
			_ASSERTE(false);
	}
}
//Register a pointer to a recreated object which has not been created yet
void LoadData::RegisterHandleToLoad(uint64_t original, void ** handleToLoad) {
	loadhandle lh;
	lh.handleToLoad = handleToLoad;
	lh.originalHandle = original;
	handlesToLoad.push_back(lh);
}

void * LoadData::RetrieveConstructedHandle(uint64_t original) {
	//If the handle is missing then the user forgot to register the object
	//they constructed using the necessary REPAIR_HANDLE(variable)
	_ASSERTE(handlesRebuilt.find(original) != handlesRebuilt.end());

	return handlesRebuilt[original];
}

//Use the list of built handles and the
//originalHandles to match created objects
//to old pointers to created objects
//and patch up the pointers to point to the new objects
void LoadData::FinishLoading() {
	for (auto handleToPatch : handlesToLoad) {
		//Verify the handle was built and registered
		//If no object was built you can't fix the handle, and there's a problem!
		_ASSERTE(handlesRebuilt.find(handleToPatch.originalHandle) != handlesRebuilt.end());

		//Search for the handle
		map<uint64_t,void*>::iterator handle = handlesRebuilt.find(handleToPatch.originalHandle);
		
		if (handle == handlesRebuilt.end())
			//If you fail to restore the handle, set it to NULL
			*handleToPatch.handleToLoad = NULL;
		else
			//Restore the pointer to a new valid value
			*handleToPatch.handleToLoad = handlesRebuilt[handleToPatch.originalHandle];
	}
	handlesToLoad.clear();
}
