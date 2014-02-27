#include "stdafx.h"
#include "ReflectionData.h"

//Register a recreated object
void LoadData::RegisterLoadedHandle(void * original, void * newHandle) {
	//If the handle is being rebuilt twice, we have a problem
	_ASSERTE(handlesRebuilt.find(original) == handlesRebuilt.end());
	handlesRebuilt[original] = newHandle;
}
//Register a pointer to a recreated object which has not been created yet
void LoadData::RegisterHandleToLoad(void * original, void ** handleToLoad) {
	loadhandle lh;
	lh.handleToLoad = handleToLoad;
	lh.originalHandle = original;
	handlesToLoad.push_back(lh);
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

		//Restore the pointer to a new valid value
		*handleToPatch.handleToLoad = handlesRebuilt[handleToPatch.originalHandle];
	}
}

void Savable::SaveValue(ReflectionData::savable valueData,Json::Value & parentValue) {
	switch (valueData.dataType) {
	case ReflectionData::SAVE_INT8:
		parentValue[valueData.memberName] = *(int8_t*)valueData.member;
		return;
	case ReflectionData::SAVE_UINT8:
		parentValue[valueData.memberName] = *(uint8_t*)valueData.member;
		return;
	case ReflectionData::SAVE_INT16:
		parentValue[valueData.memberName] = *(int16_t*)valueData.member;
		return;
	case ReflectionData::SAVE_UINT16:
		parentValue[valueData.memberName] = *(uint16_t*)valueData.member;
		return;
	case ReflectionData::SAVE_INT32:
		parentValue[valueData.memberName] = *(int32_t*)valueData.member;
		return;
	case ReflectionData::SAVE_UINT32:
		parentValue[valueData.memberName] = *(uint32_t*)valueData.member;
		return;
	case ReflectionData::SAVE_INT64:
		parentValue[valueData.memberName] = *(int64_t*)valueData.member;
		return;
	case ReflectionData::SAVE_UINT64:
		parentValue[valueData.memberName] = *(uint64_t*)valueData.member;
		return;
	case ReflectionData::SAVE_FLOAT:
		parentValue[valueData.memberName] = *(float*)valueData.member;
		return;
	case ReflectionData::SAVE_DOUBLE:
		parentValue[valueData.memberName] = *(double*)valueData.member;
		return;
	case ReflectionData::SAVE_STRING:
		parentValue[valueData.memberName] = *(string*)valueData.member;
		return;
	//Saves the handle: but does not save what the handle points to
	case ReflectionData::SAVE_HANDLE:
		if (sizeof(void*) == 4)
			//32 bit
			parentValue[valueData.memberName] = (uint32_t)*(uint32_t*)valueData.member;
		else
			//Assume 64 bit
			//Use only the leas significant 32 bits and hope that's enough
			parentValue[valueData.memberName] = (uint32_t)*(uint64_t*)valueData.member;
		return;
	//Saves the handle: and saves what it points to
	//will reconstruct the object automatically upon load
	case ReflectionData::SAVE_OWNEDHANDLE:
	case ReflectionData::SAVE_LIST:
	case ReflectionData::SAVE_VECTOR:
	case ReflectionData::SAVE_CONTIGOUSLIST:
	case ReflectionData::SAVE_STATICARRAY:
	}
}

void Savable::LoadValue(ReflectionData::savable valueData,Json::Value & parentValue, LoadData & loadData) {
	if (!parentValue.isMember(valueData.memberName)) {
		cout << "Member: " << valueData.memberName << " missing.\n";
		return;
	}
	switch (valueData.dataType) {
	case ReflectionData::SAVE_INT8:
		*(int8_t*)valueData.member = (int8_t)parentValue.get(valueData.memberName,*(int8_t*)valueData.member).asInt();
		return;
	case ReflectionData::SAVE_UINT8:
		*(uint8_t*)valueData.member = (uint8_t)parentValue.get(valueData.memberName,*(uint8_t*)valueData.member).asUInt();
		return;
	case ReflectionData::SAVE_INT16:
		*(int16_t*)valueData.member = (int16_t)parentValue.get(valueData.memberName,*(int16_t*)valueData.member).asInt();
		return;
	case ReflectionData::SAVE_UINT16:
		*(uint16_t*)valueData.member = (uint16_t)parentValue.get(valueData.memberName,*(uint16_t*)valueData.member).asUInt();
		return;
	case ReflectionData::SAVE_INT32:
		*(int32_t*)valueData.member = (int32_t)parentValue.get(valueData.memberName,*(int32_t*)valueData.member).asInt();
		return;
	case ReflectionData::SAVE_UINT32:
		*(uint32_t*)valueData.member = (uint32_t)parentValue.get(valueData.memberName,*(uint32_t*)valueData.member).asUInt();
		return;
	case ReflectionData::SAVE_INT64:
		*(int64_t*)valueData.member = (int64_t)parentValue.get(valueData.memberName,*(int64_t*)valueData.member).asInt64();
		return;
	case ReflectionData::SAVE_UINT64:
		*(uint64_t*)valueData.member = (uint64_t)parentValue.get(valueData.memberName,*(uint64_t*)valueData.member).asUInt64();
		return;
	case ReflectionData::SAVE_FLOAT:
		*(float*)valueData.member = (float)parentValue.get(valueData.memberName,*(float*)valueData.member).asFloat();
		return;
	case ReflectionData::SAVE_DOUBLE:
		*(double*)valueData.member = (double)parentValue.get(valueData.memberName,*(double*)valueData.member).asDouble();
		return;
	case ReflectionData::SAVE_STRING:
		*(string*)valueData.member = parentValue.get(valueData.memberName,*(string*)valueData.member).asString();
		return;
	//Saves the handle: but does not save what the handle points to
	case ReflectionData::SAVE_HANDLE: {
		void * loadedValue;
		loadedValue = (void*)parentValue.get(valueData.memberName,0).asUInt();
		if (loadedValue != NULL)
			//Register it to be connected to an actual class

		return;
	}

	//Saves the handle: and saves what it points to
	//will reconstruct the object automatically upon load
	case ReflectionData::SAVE_OWNEDHANDLE:
	case ReflectionData::SAVE_LIST:
	case ReflectionData::SAVE_VECTOR:
	case ReflectionData::SAVE_CONTIGOUSLIST:
	case ReflectionData::SAVE_STATICARRAY:
	}
}

void Savable::Save(Json::Value & parentValue) {
	//load reflection information for this class
	vector<ReflectionData::savable> members = ReflectionStore::Data().LookupClassMembers(Name());
	//save every member of the class
	for (auto member : members)
		SaveValue(member,parentValue);
}

void Savable::Load(Json::Value & parentValue, LoadData & loadData) {
	//load reflection information for this class
	vector<ReflectionData::savable> members = ReflectionStore::Data().LookupClassMembers(Name());
	//Load every member of the class
	for (auto member : members)
		LoadValue(member,parentValue,loadData);
}