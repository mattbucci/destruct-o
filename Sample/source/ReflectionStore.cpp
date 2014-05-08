#include "stdafx.h"
#include "ReflectionStore.h"



ReflectionStore & ReflectionStore::Data() {
	static ReflectionStore allReflectionData;
	return allReflectionData;
}


void ReflectionStore::RegisterConstructableClass(string name, reflector* reflectionInformation) {
	_ASSERTE(reflectionStore.find(name) == reflectionStore.end());

	reflectionStore[name] = reflectionInformation;
}

reflector * ReflectionStore::RetrieveClassTypeTools(string name) {
	auto iterator = reflectionStore.find(name);
	//Unable to find an appropriate constructor
	_ASSERTE(iterator != reflectionStore.end());
	if (iterator == reflectionStore.end())
		return NULL;

	//Create a class of the instance requested
	return iterator->second;
}