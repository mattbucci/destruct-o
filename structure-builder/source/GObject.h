#pragma once
#include "stdafx.h"

typedef uint32_t GID;
typedef uint32_t TID;

class GObject {
	GID id;
	static uint32_t currentMaxId;
public:
	GObject(void);
	virtual ~GObject(void);

	//Public copy constructors so inheritance doesn't have problems, but causes
	//assert on execution
	GObject(const GObject & other);
	virtual GObject & operator=(const GObject & other);

	//Get the name of the type of this class
	virtual string GetName() = 0;

	//Returns the name and id for debug purposes
	virtual string GetTypeName();

	//Get the id of this GObject instance
	GID GetId();

};

