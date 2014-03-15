#include "stdafx.h"
#include "ReflectionData.h"
#include "ContiguousList.h"

//Register a recreated object
void LoadData::RegisterLoadedHandle(uint64_t original, void * newHandle) {
	//If the handle is being rebuilt twice, we have a problem
	_ASSERTE(handlesRebuilt.find(original) == handlesRebuilt.end());
	handlesRebuilt[original] = newHandle;
}
//Register a pointer to a recreated object which has not been created yet
void LoadData::RegisterHandleToLoad(uint64_t original, void ** handleToLoad) {
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

void Savable::SaveValue(ReflectionData::savable valueData,Json::Value & value) {
	switch (valueData.dataType) {
	case ReflectionData::SAVE_INT8:
		value = *(int8_t*)valueData.member;
		return;
	case ReflectionData::SAVE_UINT8:
		value = *(uint8_t*)valueData.member;
		return;
	case ReflectionData::SAVE_INT16:
		value = *(int16_t*)valueData.member;
		return;
	case ReflectionData::SAVE_UINT16:
		value = *(uint16_t*)valueData.member;
		return;
	case ReflectionData::SAVE_INT32:
		value = *(int32_t*)valueData.member;
		return;
	case ReflectionData::SAVE_UINT32:
		value = *(uint32_t*)valueData.member;
		return;
	case ReflectionData::SAVE_INT64:
		value = *(int64_t*)valueData.member;
		return;
	case ReflectionData::SAVE_UINT64:
		value = *(uint64_t*)valueData.member;
		return;
	case ReflectionData::SAVE_FLOAT:
		value = *(float*)valueData.member;
		return;
	case ReflectionData::SAVE_DOUBLE:
		value = *(double*)valueData.member;
		return;
	case ReflectionData::SAVE_STRING:
		value = *(string*)valueData.member;
		return;
	//Saves the handle: but does not save what the handle points to
	case ReflectionData::SAVE_HANDLE:
		if (sizeof(void*) == 4)
			//32 bit
			//Everything stored as 64 bit integers
			value = (uint64_t)*(uint32_t*)valueData.member;
		else
			//Assume 64 bit
			value = *(uint64_t*)valueData.member;
		return;
	//Saves the handle: and saves what it points to
	//will reconstruct the object automatically upon load
	case ReflectionData::SAVE_OWNEDHANDLE: {
		//Retrieve the pointer value
		uint64_t handle;
		if (sizeof(void*) == 4)
			//32 bit
			//Everything stored as 64 bit integers
			handle = (uint64_t)*(uint32_t*)valueData.member;
		else
			//Assume 64 bit
			handle = *(uint64_t*)valueData.member;
		//Save the new class with some internal information about its original handle
		value["__HANDLE__"] = handle;
		value["__TYPE__"] = (*(Savable**)valueData.member)->Name();
		(*(Savable**)valueData.member)->Save(value);
		return;
	}
	case ReflectionData::SAVE_INSTANCE: {
		//Retrieve the pointer value
		uint64_t handle;
		if (sizeof(void*) == 4)
			//32 bit
			//Everything stored as 64 bit integers
			handle = (uint32_t)valueData.member;
		else
			//Assume 64 bit
			handle = (uint64_t)valueData.member;
		//Save the new class with some internal information about its original handle
		value["__HANDLE__"] = handle;
		value["__TYPE__"] = ((Savable*)valueData.member)->Name();
		((Savable*)valueData.member)->Save(value);
		return;
	}

	case ReflectionData::SAVE_LIST:
		SaveContainerValue<list,allocator>(valueData,value);
		return;
	case ReflectionData::SAVE_VECTOR:
		SaveContainerValue<vector,allocator>(valueData,value);
		return;
	case ReflectionData::SAVE_CONTIGOUSLIST:
		SaveContainerValue<ContiguousList,__listdummyallocator>(valueData,value);
		return;
	default:
		_ASSERTE(false);
	}
}

void Savable::LoadValue(ReflectionData::savable valueData,Json::Value & value, LoadData & loadData) {
	switch (valueData.dataType) {
	case ReflectionData::SAVE_INT8:
		*(int8_t*)valueData.member = (int8_t)value.asInt();
		return;
	case ReflectionData::SAVE_UINT8:
		*(uint8_t*)valueData.member = (uint8_t)value.asUInt();
		return;
	case ReflectionData::SAVE_INT16:
		*(int16_t*)valueData.member = (int16_t)value.asInt();
		return;
	case ReflectionData::SAVE_UINT16:
		*(uint16_t*)valueData.member = (uint16_t)value.asUInt();
		return;
	case ReflectionData::SAVE_INT32:
		*(int32_t*)valueData.member = (int32_t)value.asInt();
		return;
	case ReflectionData::SAVE_UINT32:
		*(uint32_t*)valueData.member = (uint32_t)value.asUInt();
		return;
	case ReflectionData::SAVE_INT64:
		*(int64_t*)valueData.member = (int64_t)value.asInt64();
		return;
	case ReflectionData::SAVE_UINT64:
		*(uint64_t*)valueData.member = (uint64_t)value.asUInt64();
		return;
	case ReflectionData::SAVE_FLOAT:
		*(float*)valueData.member = (float)value.asFloat();
		return;
	case ReflectionData::SAVE_DOUBLE:
		*(double*)valueData.member = (double)value.asDouble();
		return;
	case ReflectionData::SAVE_STRING:
		*(string*)valueData.member = value.asString();
		return;
	//Saves the handle: but does not save what the handle points to
	case ReflectionData::SAVE_HANDLE: {
		uint64_t loadedValue;
		loadedValue = value.get(valueData.memberName,0).asUInt64();
		if (loadedValue != 0)
			//Register it to be connected to an actual class
			loadData.RegisterHandleToLoad(loadedValue,(void**)valueData.member);
		else
			//Just set it NULL
			*(void**)valueData.member = nullptr;
		return;
	}

	//Saves the handle: and saves what it points to
	//will reconstruct the object automatically upon load
	case ReflectionData::SAVE_OWNEDHANDLE: {
		uint64_t loadedValue;
		loadedValue = value["__HANDLE__"].get(valueData.memberName,0).asUInt64();
		if (loadedValue != 0) {
			Savable * classInstance = ReflectionStore::Data().RetrieveClassInstance(value["__TYPE__"].asString());
			//Save the fact you constructed a class so that anything else
			//that was supposed to have a handle to that class can have their handles reconstructed
			loadData.RegisterLoadedHandle(loadedValue,classInstance);
			//Load the class
			classInstance->Load(value,loadData);
		}
		else
			//Just set it NULL
			*(void**)valueData.member = nullptr;
		return;
	}

	case ReflectionData::SAVE_INSTANCE: {
		uint64_t loadedValue;
		loadedValue = value["__HANDLE__"].get(valueData.memberName,0).asUInt64();
		if (loadedValue != 0) {
			//Save the fact you constructed a class so that anything else
			//that was supposed to have a handle to that class can have their handles reconstructed
			loadData.RegisterLoadedHandle(loadedValue,(Savable*)valueData.member);
			//Load the class
			((Savable*)valueData.member)->Load(value,loadData);
		}
		else {
			cout << "Handle was NULL on SAVE_INSTANCE...\n";
			_ASSERTE(false);
		}
		return;
	}
	case ReflectionData::SAVE_LIST:
		LoadContainerValue<list,allocator>(valueData,value,loadData);
		return;
	case ReflectionData::SAVE_VECTOR:
		LoadContainerValue<vector,allocator>(valueData,value,loadData);
		return;
	case ReflectionData::SAVE_CONTIGOUSLIST:
		LoadContainerValue<ContiguousList,allocator>(valueData,value,loadData);
		return;
	default:
		_ASSERTE(false);
	}
}



void Savable::Save(Json::Value & parentValue) {
	//load reflection information for this class
	vector<ReflectionData::savable> members = ReflectionStore::Data().LookupClassMembers(this);
	//save every member of the class
	for (auto member : members)
		SaveValue(member,parentValue[member.memberName]);
}

void Savable::Load(Json::Value & parentValue, LoadData & loadData) {
	//load reflection information for this class
	vector<ReflectionData::savable> members = ReflectionStore::Data().LookupClassMembers(this);
	//Load every member of the class
	for (auto member : members) {
		if (!parentValue.isMember(member.memberName))
			cout << "Member: " << member.memberName << " missing.\n";
		else
			LoadValue(member,parentValue[member.memberName],loadData);
	}
}


ReflectionStore & ReflectionStore::Data() {
	static ReflectionStore allReflectionData;
	return allReflectionData;
}



void ReflectionStore::RegisterClassType(string name, ReflectionData * data) {
	//Check that a class isn't being registered twice
	_ASSERTE(reflectionStore.find(name) == reflectionStore.end());
	//Save the reflection information
	reflectionStore[name] = data;
	inheritanceTable[name] = vector<string>(0);
}
void ReflectionStore::RegisterInheritance(string derivingClass, string baseClass) {
	inheritanceTable[derivingClass].push_back(baseClass);
}

void ReflectionStore::GetDataFromClass(string classname,Savable * classToLookup, vector<ReflectionData::savable> & members) {
	_ASSERTE(reflectionStore.find(classname) != reflectionStore.end());

	//Retrieve the members of this class which can be saved
	vector<ReflectionData::savable> savableMembers = reflectionStore[classname]->RetrieveReflectionData(classToLookup);
	//Add this classes data
	members.insert(members.end(),savableMembers.begin(),savableMembers.end());

	//All the classes which make up this one class (excluding itself)
	vector<string> & classes = inheritanceTable[classname];
	//recurse over all classes this class inherits from
	for (auto iclass : classes)
		GetDataFromClass(iclass,classToLookup,members);
}

vector<ReflectionData::savable> ReflectionStore::LookupClassMembers(Savable * classToLookup) {
	vector<ReflectionData::savable> allMembers;
	//Call the recursive function to retrieve the members of this class and everything in inherits from
	GetDataFromClass(classToLookup->Name(),classToLookup,allMembers);

	return allMembers;

}
Savable * ReflectionStore::RetrieveClassInstance(string name) {
	auto iterator = reflectionStore.find(name);
	//Unable to find an appropriate constructor
	if (iterator == reflectionStore.end())
		return NULL;

	ReflectionData * refData = iterator->second;

	//check that a constructor was supplied for this type
	if (!refData->constructor)
		return NULL;
	
	
	//Create a class of the instance requested
	return refData->constructor();
}

//Serialize data
vector<unsigned char> Savable::Serialize(Savable * classToSerialize) {
	Json::Value root;
	//Serialize the class into the root value
	classToSerialize->Save(root);
	//For now use a styled writer so we can look at pretty json
	Json::StyledWriter writer;
	string json = writer.write(root);
	//Copy to a vector
	vector<unsigned char> rawData(json.size());
	for (int i = 0; i < json.size();i++)
		rawData[i] = json[i];

	return rawData;
}

//Deserialize data into a new class and return it
Savable * Savable::Deserialize(vector<unsigned char> serializedData) {
	Json::Reader reader;
	Json::Value root;
	reader.readArray(
}
	
//Deserialize data into an existing class
Savable * Savable::Deserialize(vector<unsigned char> serializedData, Savable * saveInto) {

}