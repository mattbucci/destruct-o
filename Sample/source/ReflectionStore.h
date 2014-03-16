
#pragma once

#include "stdafx.h"
#include "Savable.h"


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