
#pragma once

#include "stdafx.h"
#include "ContiguousList.h"
#include <json/json.h>

#include <list>

class Savable;

namespace ReflectionData {
	//The different types which can be saved
	enum SaveType {
		SAVE_BOOL,
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
		SAVE_VEC2,
		SAVE_VEC3,
		SAVE_VEC4,
		//Saves the handle, but does not save what the handle points to
		SAVE_HANDLE,
		//Saves the handle, and saves what it points to
		//will reconstruct the object automatically upon load
		SAVE_OWNEDHANDLE,
		//Exactly like the above except the constructor of the savable is expected
		//to automatically repair the OWNEDHANDLE by itself
		//though the object is still created/destroyed by the save system
		//and SAVE_HANDLE will still be repaired automatically
		SAVE_OWNEDHANDLEAR,
		//Exactly like the above except the object is not created/destroyed
		//the user must do that himself
		//and during Load() REPAIR_HANDLE(variable) should be called so that SAVE_HANDLE
		//to the object are repaired correctly
		SAVE_USEROWNEDHANDLE,
		//Save an instance of something savable, (not a pointer)
		SAVE_INSTANCE,
		SAVE_LIST,
		SAVE_VECTOR,
		SAVE_CONTIGOUSLIST,
		//Do not use!
		SAVE_NOTHING,
	};
	struct savable {
		savable(SaveType type, void * member, string name) {
			this->dataType = type;
			this->member = member;
			this->memberName = name;
		}
		savable(SaveType type, void * member, string name, SaveType internalType) {
			this->dataType = type;
			this->member = member;
			this->memberName = name;
			this->internalType = internalType;
		}
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



#define CLASS_CONCAT_B(a,b) a##b
#define CLASS_CONCAT_A(a,b) CLASS_CONCAT_B(a,b)
#define CLASS_TNAME(classname) CLASS_CONCAT_A(classname,_TypeData)
#define CLASS_VNAME(classname) CLASS_CONCAT_A(classname,_TypeDataStaticValue)




#define CLASS_DECLARATION(classname) public: string Name() {return #classname;} protected: \
								virtual void savesystem_reflect(vector<ReflectionData::savable> & reflectMembersInto) { 


#define INHERITS_FROM(derived) derived::savesystem_reflect(reflectMembersInto);

#define CLASS_MEMBER(member,type) reflectMembersInto.push_back(ReflectionData::savable(type,&member,#member));
#define CLASS_CONTAINER_MEMBER(member,containerType,type) reflectMembersInto.push_back(ReflectionData::savable(containerType,&member,#member,type));

#define END_DECLARATION } private:



#define CLASS_SAVE_CONSTRUCTOR(classname) struct CLASS_TNAME(classname) { CLASS_TNAME(classname)(){ReflectionStore::Data().RegisterConstructableClass(#classname,[](){return new classname();});}}; \
	static CLASS_TNAME(classname) CLASS_VNAME(classname);
