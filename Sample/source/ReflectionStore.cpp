#include "stdafx.h"
#include "ReflectionStore.h"



ReflectionStore & ReflectionStore::Data() {
	static ReflectionStore allReflectionData;
	return allReflectionData;
}


void ReflectionStore::RegisterConstructableClass(string name, function<Savable*()> constructor) {
	_ASSERTE(reflectionStore.find(name) == reflectionStore.end());

	reflectionStore[name] = constructor;
}

Savable * ReflectionStore::RetrieveClassInstance(string name) {
	auto iterator = reflectionStore.find(name);
	//Unable to find an appropriate constructor
	if (iterator == reflectionStore.end())
		return NULL;

	//Create a class of the instance requested
	return iterator->second();
}
