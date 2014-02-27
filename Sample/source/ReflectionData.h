
#pragma once

#include "stdafx.h"
#include "GObject.h"
#include "ContiguousList.h"
#include <json/json.h>

class LoadData {
	struct loadhandle {
		void ** handleToLoad;
		void * originalHandle;
	};
	vector<loadhandle> handlesToLoad;
	//matches an original handle to a created handle
	map<void*,void*> handlesRebuilt;
public:
	//Register a recreated object
	void RegisterLoadedHandle(void * original, void * newHandle);
	//Register a pointer to a recreated object which has not been created yet
	void RegisterHandleToLoad(void * original, void ** handleToLoad);

	//Use the list of built handles and the
	//originalHandles to match created objects
	//to old pointers to created objects
	//and patch up the pointers to point to the new objects
	void FinishLoading();
};


class ReflectionData {
public:
	//The different types which can be saved
	enum SaveType {
		SAVE_INT8,
		SAVE_UINT8,
		SAVE_INT16,
		SAVE_UINT16,
		SAVE_INT32,
		SAVE_UINT32,
		SAVE_INT64,
		SAVE_UINT64,
		SAVE_FLOAT,
		SAVE_DOUBLE,
		SAVE_STRING,
		//Saves the handle, but does not save what the handle points to
		SAVE_HANDLE,
		//Saves the handle, and saves what it points to
		//will reconstruct the object automatically upon load
		SAVE_OWNEDHANDLE,
		SAVE_LIST,
		SAVE_VECTOR,
		SAVE_CONTIGOUSLIST,
		SAVE_STATICARRAY,
	};
	struct savable {
		savable(SaveType type, void * member, string name);
		//The type of data to save/load
		SaveType dataType;
		//a pointer to the member
		void * member;
		//The name of the member
		string memberName;
	};


	virtual ~ReflectionData();

	virtual vector<savable> RetrieveReflectionData(void * classdata) = 0;
};


class Savable {
	Savable(){}
	virtual ~Savable(){}

	void SaveValue(ReflectionData::savable valueData,Json::Value & parentValue);

	void LoadValue(ReflectionData::savable valueData,Json::Value & parentValue, LoadData & loadData);

protected:
	/*//Saves standard types automatically
	template<T>
	void SaveValue(Json::Value classObject,string name, T * valueToSave) {
		classObject[name] = *valueToSave;
	}
	//Loads standard types automatically
	template<T>
	void LoadValue(Json::Value classObject,string name, T * valueToLoad) {

	}*/

	virtual string Name() = 0;

	virtual void Save(Json::Value & parentValue);

	virtual void Load(Json::Value & parentValue, LoadData & loadData);

public:

};


class ReflectionStore {
public:
	static ReflectionStore & Data();

	void RegisterClassType(string name, ReflectionData * data);
	void RegisterInheritance(string derivingClass, string baseClass);

	vector<ReflectionData::savable> LookupClassMembers(string classname);
};

#define CLASS_CONCAT_B(a,b) a##b
#define CLASS_CONCAT_A(a,b) CLASS_CONCAT_B(a,b)
#define CLASS_TNAME(classname) CLASS_CONCAT_A(classname,_TypeData)


#define CLASS_DECLARATION(classname) class CLASS_TNAME : public ReflectionData { \
					ReflectionData(){ \
						string name = #classname;\
						ReflectionStore::Data().RegisterClassType(name,this);\
					}\
					vector<savable> RetrieveReflectionData(void * classdata) {\
						vector<savable> members;	\
						classname * instance = (classname*)classdata;
//#define INHERITS_FROM(otherclassname) ReflectionStroe::Data().RegisterInheritance(name,#otherclassname);
#define CLASS_MEMBER(member,type) members.push_back(savable(type,&instance->member,#member));

#define END_CLASS_DECLARATION return members;}}



