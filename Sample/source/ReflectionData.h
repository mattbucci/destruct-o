
#pragma once

#include "stdafx.h"
#include "GObject.h"
#include "ContiguousList.h"
#include <json/json.h>



//If the GObject will need to be linked afterwards
struct Linkable {
	GObject * *linkAt;
	//Replace *linkAt with the GObject with id "linkId"
	GID linkId;
};

//Used to store data about a class to save that class
class ReflectionData {
	friend class SObject;

	//The save and load types are lambda expressions
	//save() takes a vector to add the saved data to
	typedef function<void(vector<int8_t> & saveTo)> saveFunction;
	//load() takes an array (loadFrom) to load the values from, linkRequirements to add any found GObject handles to and
	//should return the number of bytes used during load
	typedef function<int32_t(int8_t * loadFrom,list<Linkable> & linkRequirements)> loadFunction;

	//Holds the data needed to save/load a single value
	class Reflection {
	public:
		//The save operation
		saveFunction save;
		//The load operation, also adds this reflection to the sob
		loadFunction load;

	};

	//The constructed list representing the reflected contents of the class
	list<Reflection> allSavables;


	//For fast primitive loading, loads a value from the top of loadFrom saving it to loadTo
	//returns the size of the value loaded. linkRequirement not used in this overload
	template<class T>
	static void Load(T* loadTo, string name, Json::Value default, Json::Value & loadFrom, list<Linkable> & linkRequirements) {
		loadFrom.get(name,default).as
	}
	//Save the value into JSON
	template<class T>
	static void Save(T & value, string valueName, vector<int8_t> & saveTo) {
		int8_t * readAt = (int8_t*)&value;
		for (int i = 0; i < sizeof(T); i++)
			saveTo.push_back(readAt[i]);
	}

	//For loading game objects only
	//same as primitive Load() except it adds loadTo to linkRequirements
	static void Load(GObject** loadTo, string name, Json::Value & loadFrom, list<Linkable> & linkRequirements) {
		//Load the game handle
		GID objectHandle;
		Load(&objectHandle,name,loadFrom,linkRequirements);
		//Add the value to be linked in the future
		Linkable link;
		link.linkAt = loadTo;
		link.linkId = objectHandle;
		linkRequirements.push_back(link);
		//Set the value to be null for now 
		*loadTo = NULL;
	}
	//For saving game objects only
	static void Save(GObject * gameObjectHandle, vector<int8_t> & saveTo) {
		//Save handle
		Save(gameObjectHandle->GetId(),saveTo);
	}
	

public:

	//Create a new empty reflectionData
	ReflectionData();

	//Cleanup all the offsets
	~ReflectionData();

	//Add save/load reflection data for a member of the desired class
	void AddSavable(saveFunction saver, loadFunction loader);


	//static Functions used during save/load below

//Macros for making my job a little easier for container types
#define SAVECONTAINER(container) template<class T> \
	static void SaveContainerValue(container<T> * value, vector<int8_t> & saveTo) {\
		Save((uint32_t)value->size(),saveTo);\
		for (T & each : value)\
			Save(value,saveTo);\
	}
#define LOADCONTAINER(container) 	template<class T> \
	static int32_t LoadContainerValue(container<T> * value, int8_t * loadFrom, list<Linkable> & linkRequirements) {\
		uint32_t length;\
		int32_t size = Load(&length,loadFrom,linkRequirements);\
		value->clear();\
		for (uint32_t i = 0; i < length; i++) {\
			T item;\
			size += Load(&item,loadFrom+size,linkRequirements);\
			value->push_back(item);\
		}\
		return size;\
	}

	template<class T>
	static void SaveArrayValue(T ** value, int32_t length, vector<int8_t> & saveTo) {
		//Save the contents. Length is static
		for (int i = 0; i < length; i++)
			Save((*value)[i],saveTo);
	}

	//For containers
	SAVECONTAINER(vector)
	SAVECONTAINER(list)
	SAVECONTAINER(ContiguousList)
	//SAVECONTAINER(map) //pending

	LOADCONTAINER(vector)
	LOADCONTAINER(list)
	LOADCONTAINER(ContiguousList)
	//LOADCONTAINER(map) //pending

	template<class T>
	static int32_t LoadArrayValue(T ** value, int length, int8_t * loadFrom, list<Linkable> & linkRequirements) {\
		for (uint32_t i = 0; i < length; i++) {
			T item;
			size += Load(&T,loadFrom+size,linkRequirements);
			value[i] = item;
		}
		return size;
	}

//Don't pollute with #defines we don't need
#undef SAVECONTAINER
#undef LOADCONTAINER

};

//Use the above values to allow simple creation of a save/load table by the user
//This system heavily abuses lambda expressions, go no further without a solid understanding!

//static int8_t RegisterType(string typeName, function<GObject*(void)> toRegister);
#define SAVABLE_CLASS(classname) int8_t classname::forceClassRegistration = RegisterType(#classname,[](){\
	return (GObject*)new classname();\
	};\
	ReflectionData classname::Reflect() { ReflectionData ref;
#define MEMBER_VALUE(value) ref.AddSavable(\
	[&,value](vector<int8_t> & saveTo) {\
		ReflectionData::Save(\
	},\
	[&,value](int8_t * loadFrom,int32_t objectlength,list<Linkable> & linkRequirements) {\
		return ReflectionData::LoadValue(&value,loadFrom,linkRequirements);\
	});
#define MEMBER_CONTAINER(container) ref.AddSavable(\
	[&,container](vector<int8_t> & saveTo) {\
		ReflectionData::SaveContainerValue(&pointer_to_container,saveTo);\
	},\
	[&,container](int8_t * loadFrom,int32_t objectlength,list<Linkable> & linkRequirements) {\
		return ReflectionData::LoadContainerValue(&pointer_to_container,loadFrom,linkRequirements);\
	});
#define MEMBER_ARRAY(array,static_array_length) ref.AddSavable(\
	[&array,static_array_length](vector<int8_t> & saveTo) {\
		ReflectionData::SaveArrayValue(&array,static_array_length,saveTo);\
	},\
	[&array,static_array_length](int8_t * loadFrom,int32_t objectlength,list<Linkable> & linkRequirements) {\
		return ReflectionData::LoadArrayValue(&array,static_array_length,loadFrom,linkRequirements);\
	});
#define END_SAVABLE_CLASS }

