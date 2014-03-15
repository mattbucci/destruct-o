
#pragma once

#include "stdafx.h"
#include "GObject.h"
#include "ContiguousList.h"
#include <json/json.h>

#include <list>

class LoadData {
	struct loadhandle {
		void ** handleToLoad;
		uint64_t originalHandle;
	};
	vector<loadhandle> handlesToLoad;
	//matches an original handle to a created handle
	map<uint64_t,void*> handlesRebuilt;
public:
	//Register a recreated object
	void RegisterLoadedHandle(uint64_t original, void * newHandle);
	//Register a pointer to a recreated object which has not been created yet
	void RegisterHandleToLoad(uint64_t original, void ** handleToLoad);

	//Use the list of built handles and the
	//originalHandles to match created objects
	//to old pointers to created objects
	//and patch up the pointers to point to the new objects
	void FinishLoading();
};

class Savable;

namespace ReflectionData {
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
		//Save an instance of something savable, (not a pointer)
		SAVE_INSTANCE,
		SAVE_LIST,
		SAVE_VECTOR,
		SAVE_CONTIGOUSLIST,
		//Do not use!
		SAVE_NOTHING,
	};
	struct savable {
		savable(SaveType type, void * member, string name);
		savable(){}
		//The type of data to save/load
		SaveType dataType;
		//a pointer to the member
		void * member;
		//If the dataType is a container or array, this type indicates
		//the internal type of the array
		SaveType internalType;
		//The name of the member
		string memberName;
	};

};


class Savable {
	Savable(){}
	virtual ~Savable(){}

	void SaveValue(ReflectionData::savable valueData,Json::Value & value);

	void LoadValue(ReflectionData::savable valueData,Json::Value & value, LoadData & loadData);
	
	template <template<class, class> class ContainerType, template<class> class AllocaterType, class InternalType>
	void LoadSpecificContainer(ReflectionData::savable valueData,Json::Value & value, LoadData & loadData) {
		//First get a pointer to the class in question
		ContainerType<InternalType,AllocaterType<InternalType>> & container =  *(ContainerType<InternalType,AllocaterType<InternalType>>*)valueData.member;
		//Now load values into the class
		for (unsigned int i = 0; i < value.size(); i++) {
			InternalType valueToLoad;

			ReflectionData::savable saveData;
			saveData.dataType = valueData.internalType;
			saveData.internalType = ReflectionData::SAVE_NOTHING;
			saveData.member = &valueToLoad;
			saveData.memberName = ""; //No name since its part of an array
			//Load the value
			LoadValue(saveData, value[i],loadData);

			//Push the value into the container
			container.push_back(valueToLoad);
		}
	}

	template <template<class, class> class ContainerType, template<class> class AllocaterType>
	void LoadContainerValue(ReflectionData::savable valueData,Json::Value & value, LoadData & loadData) {
		switch (valueData.internalType) {
		case ReflectionData::SAVE_INT8:
			LoadSpecificContainer<ContainerType,AllocaterType,int8_t>(valueData,value,loadData);
			break;
		case ReflectionData::SAVE_UINT8:
			LoadSpecificContainer<ContainerType,AllocaterType,uint8_t>(valueData,value,loadData);
			break;
		case ReflectionData::SAVE_INT16:
			LoadSpecificContainer<ContainerType,AllocaterType,int16_t>(valueData,value,loadData);
			break;
		case ReflectionData::SAVE_UINT16:
			LoadSpecificContainer<ContainerType,AllocaterType,uint16_t>(valueData,value,loadData);
			break;
		case ReflectionData::SAVE_INT32:
			LoadSpecificContainer<ContainerType,AllocaterType,int32_t>(valueData,value,loadData);
			break;
		case ReflectionData::SAVE_UINT32:
			LoadSpecificContainer<ContainerType,AllocaterType,uint32_t>(valueData,value,loadData);
			break;
		case ReflectionData::SAVE_INT64:
			LoadSpecificContainer<ContainerType,AllocaterType,int64_t>(valueData,value,loadData);
			break;
		case ReflectionData::SAVE_UINT64:
			LoadSpecificContainer<ContainerType,AllocaterType,uint64_t>(valueData,value,loadData);
			break;
		case ReflectionData::SAVE_FLOAT:
			LoadSpecificContainer<ContainerType,AllocaterType,float>(valueData,value,loadData);
			break;
		case ReflectionData::SAVE_DOUBLE:
			LoadSpecificContainer<ContainerType,AllocaterType,double>(valueData,value,loadData);
			break;
		case ReflectionData::SAVE_STRING:
			LoadSpecificContainer<ContainerType,AllocaterType,string>(valueData,value,loadData);
			break;
		//This is actually unsafe but I doubt it'll break anything
		case ReflectionData::SAVE_OWNEDHANDLE:
		case ReflectionData::SAVE_HANDLE:
			LoadSpecificContainer<ContainerType,AllocaterType,void*>(valueData,value,loadData);
			break;
		default:
			//Illegal type in container
			_ASSERTE(false);
		}
	}
	
	template <template<class, class> class ContainerType, template<class> class AllocaterType, class InternalType>
	void SaveSpecificContainer(ReflectionData::savable valueData,Json::Value & value) {
		//First get a pointer to the class in question
		ContainerType<InternalType,AllocaterType<InternalType>> & container =  *(ContainerType<InternalType,AllocaterType<InternalType>>*)valueData.member;
		//Now iterate over the class
		int i = 0;
		for (auto member : container) {
			ReflectionData::savable saveData;
			saveData.dataType = valueData.internalType;
			saveData.internalType = ReflectionData::SAVE_NOTHING;
			saveData.member = &member;
			saveData.memberName = ""; //No name since its part of an array
			SaveValue(saveData, value[i]);
			i++;
		}
	}

	template <template<class, class> class ContainerType, template<class> class AllocaterType>
	//Selects the appropriate type argument and calls SaveSpecificContainer
	void SaveContainerValue(ReflectionData::savable valueData,Json::Value & value) {
		switch (valueData.internalType) {
		case ReflectionData::SAVE_INT8:
			SaveSpecificContainer<ContainerType,AllocaterType,int8_t>(valueData,value);
			break;
		case ReflectionData::SAVE_UINT8:
			SaveSpecificContainer<ContainerType,AllocaterType,uint8_t>(valueData,value);
			break;
		case ReflectionData::SAVE_INT16:
			SaveSpecificContainer<ContainerType,AllocaterType,int16_t>(valueData,value);
			break;
		case ReflectionData::SAVE_UINT16:
			SaveSpecificContainer<ContainerType,AllocaterType,uint16_t>(valueData,value);
			break;
		case ReflectionData::SAVE_INT32:
			SaveSpecificContainer<ContainerType,AllocaterType,int32_t>(valueData,value);
			break;
		case ReflectionData::SAVE_UINT32:
			SaveSpecificContainer<ContainerType,AllocaterType,uint32_t>(valueData,value);
			break;
		case ReflectionData::SAVE_INT64:
			SaveSpecificContainer<ContainerType,AllocaterType,int64_t>(valueData,value);
			break;
		case ReflectionData::SAVE_UINT64:
			SaveSpecificContainer<ContainerType,AllocaterType,uint64_t>(valueData,value);
			break;
		case ReflectionData::SAVE_FLOAT:
			SaveSpecificContainer<ContainerType,AllocaterType,float>(valueData,value);
			break;
		case ReflectionData::SAVE_DOUBLE:
			SaveSpecificContainer<ContainerType,AllocaterType,double>(valueData,value);
			break;
		case ReflectionData::SAVE_STRING:
			SaveSpecificContainer<ContainerType,AllocaterType,string>(valueData,value);
			break;
		//This is actually unsafe but I doubt it'll break anything
		case ReflectionData::SAVE_OWNEDHANDLE:
		case ReflectionData::SAVE_HANDLE:
			SaveSpecificContainer<ContainerType,AllocaterType,void*>(valueData,value);
			break;
		default:
			//Illegal type in container
			_ASSERTE(false);
		}
	}

protected:

	virtual void Save(Json::Value & parentValue);

	virtual void Load(Json::Value & parentValue, LoadData & loadData);

public:
	//Serialize data
	static vector<unsigned char> Serialize(Savable * classToSerialize);

	//Deserialize data into a new class and return it
	static Savable * Deserialize(vector<unsigned char> serializedData);
	
	//Deserialize data into an existing class
	static Savable * Deserialize(vector<unsigned char> serializedData, Savable * saveInto);


	virtual string Name() = 0;

};

	//A constructor to build this savable
	function<Savable*()> constructor;

class ReflectionStore {
	//Map each class to its reflection data
	map<string,function<Savable*()>> reflectionStore;
public:
	static ReflectionStore & Data();

	//Register a class which can be constructed by the save system using a no-argument constructor
	void RegisterConstructableClass(string name, function<Savable*()> constructor);

	//Retrieve an instance of a class with the given name
	Savable * RetrieveClassInstance(string name);
};

#define CLASS_CONCAT_B(a,b) a##b
#define CLASS_CONCAT_A(a,b) CLASS_CONCAT_B(a,b)
#define CLASS_TNAME(classname) CLASS_CONCAT_A(classname,_TypeData)



//A class with no inheritance, and can not be reconstructed by the save system
#define CLASS_DECLARATION(classname) class CLASS_TNAME(classname) : public ReflectionData { \
					ReflectionData(){ \
						string name = #classname;\
						ReflectionStore::Data().RegisterClassType(name,this); \
					} \
					vector<savable> RetrieveReflectionData(void * classdata) {\
						vector<savable> members;	\
						classname * instance = (classname*)classdata;

//A class with inheritance that can not be reconstructed by the save system
#define CLASS_DECLARATION_I(classname) class CLASS_TNAME(classname) : public ReflectionData { \
					ReflectionData(){ \
						string name = #classname;\
						ReflectionStore::Data().RegisterClassType(name,this);
#define BEGIN_INHERITANCE
#define INHERITS_FROM(otherclassname) ReflectionStroe::Data().RegisterInheritance(name,#otherclassname);
#define END_INHERITANCE } \
					vector<savable> RetrieveReflectionData(void * classdata) {\
						vector<savable> members;	\
						classname * instance = (classname*)classdata;

//A class with no inheritance, and can be reconstructed by the save system
#define CLASS_DECLARATION_S(classname) class CLASS_TNAME(classname) : public ReflectionData { \
					ReflectionData(){ \
						string name = #classname;\
						ReflectionStore::Data().RegisterClassType(name,this); \
						constructor = []{return (Savable*)new classname();}; \
					} \
					vector<savable> RetrieveReflectionData(void * classdata) {\
						vector<savable> members;	\
						classname * instance = (classname*)classdata;

//A class with inheritance that can be reconstructed by the save system
#define CLASS_DECLARATION_IS(classname) class CLASS_TNAME(classname) : public ReflectionData { \
					ReflectionData(){ \
						string name = #classname;\
						ReflectionStore::Data().RegisterClassType(name,this); \
						constructor = []{return (Savable*)new classname();}; \

#define CLASS_MEMBER(member,type) members.push_back(savable(type,&instance->member,#member));

#define END_CLASS_DECLARATION return members;}}

#define SAVABLE_REFLECTION_DATA(classname) static CLASS_TNAME(classname) __InternalReflectionData;



