
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
		SAVE_VEC2I,
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

	template <class T>
	inline SaveType findReflectionType() {
		_ASSERTE(false);
		return SAVE_NOTHING;
	}
	//For primitives
	template <>
	inline SaveType findReflectionType<float>() {
		return SAVE_FLOAT;
	}
	template <>
	inline SaveType findReflectionType<double>() {
		return SAVE_DOUBLE;
	}
	template <>
	inline SaveType findReflectionType<int>() {
		return SAVE_INT32;
	}
	template <>
	inline SaveType findReflectionType<vec2>() {
		return SAVE_VEC2;
	}
	template <>
	inline SaveType findReflectionType<vec3>() {
		return SAVE_VEC3;
	}
	template <>
	inline SaveType findReflectionType<vec4>() {
		return SAVE_VEC4;
	}

};


//Helper defines, dont' use
#define CLASS_CONCAT_B(a,b) a##b
#define CLASS_CONCAT_A(a,b) CLASS_CONCAT_B(a,b)
#define CLASS_TNAME(classname) CLASS_CONCAT_A(classname,_TypeData)
#define CLASS_VNAME(classname) CLASS_CONCAT_A(classname,_TypeDataStaticValue)
#define CLASS_CONCAT3_B(a,b,c) a##b##c
#define CLASS_CONCAT3_A(a,b,c) CLASS_CONCAT3_B(a,b,c)


//This should be at the start of a save block inside any savable class
//savable classes must inherit Savable, or inherit something else that does
#define CLASS_DECLARATION(classname) public: string Name() {return #classname;} protected: \
								virtual void savesystem_reflect(vector<ReflectionData::savable> & reflectMembersInto) { 

//If the current class inherits from another class, this must be in the save block indicating that class
#define INHERITS_FROM(derived) derived::savesystem_reflect(reflectMembersInto);

//Declare a member of a specific type (Look in ReflectionData:: for the types)
#define CLASS_MEMBER(member,type) reflectMembersInto.push_back(ReflectionData::savable(type,&member,#member));
//Declare a member of a container type
#define CLASS_CONTAINER_MEMBER(member,containerType,type) reflectMembersInto.push_back(ReflectionData::savable(containerType,&member,#member,type));

//End the savable declaration block
#define END_DECLARATION } private:


//If the class can be constructed into from a handle by the save system, CLASS_SAVE_CONSTRUCTOR(yourclass) must be in the .cpp file
#define CLASS_SAVE_CONSTRUCTOR(classname) struct CLASS_TNAME(classname) { \
		CLASS_TNAME(classname)(){ \
			ReflectionStore::Data().RegisterConstructableClass(#classname,new savablereflector<classname>(NULL)); \
		}; \
	}; \
	static CLASS_TNAME(classname) CLASS_VNAME(classname);

//If the class INSTANCE is within containers, call this version instead of the above to add container support (not needed for containers containing handles)
#define CLASS_SAVE_CONSTRUCTOR_CONTAINERS(classname) struct CLASS_TNAME(classname) { \
		CLASS_TNAME(classname)(){ \
			ReflectionStore::Data().RegisterConstructableClass(#classname,new savablereflector<classname>(new savablereflector<classname>::container_reflector_imp<classname>())); \
		}; \
	}; \
	static CLASS_TNAME(classname) CLASS_VNAME(classname);

//If your class contains one or more members which are user owned handles, you must overload Load and call REPAIR_HANDLE for each user owned handle before calling Savable::Load
#define REPAIR_HANDLE(variable) loadData.RegisterLoadedHandle(parentValue[#variable]["__HANDLE__"].asInt64(),variable);


//Defines a savable pair of two types
//use _DECLARATION in a header file
//use _IMPLEMENTATION in a cpp file
#define SAVABLE_PAIR_DECLARATION(pairtypename,typea,savetypea,typeb,savetypeb) class pairtypename : public Savable { \
public: \
	pairtypename() {} \
	pairtypename(typea _first, typeb _second) : first(_first), second(_second) {} \
	typea first; \
	typeb second; \
	CLASS_DECLARATION(SAVABLE_PAIR_TYPE(typea,typeb))	\
		CLASS_MEMBER(first,savetypea) \
		CLASS_MEMBER(second,savetypeb) \
	END_DECLARATION \
}
#define SAVABLE_PAIR_IMPLEMENTATION(pairtypename) CLASS_SAVE_CONSTRUCTOR_CONTAINERS(pairtypename)