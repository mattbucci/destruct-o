
#pragma once

#include "stdafx.h"
#include "Savable.h"

//Handles all class-specific reflection tools
class reflector {
public:
	//Build an instance of the class
	virtual Savable * BuildInstance() = 0;
	//Clear a container holding this type
	//and resize it to hold the given quantity of items
	virtual void ClearContainer(void * container, ReflectionData::SaveType containerType, int resizeCapacity) = 0;
	//Convert a container of this type into a vector of savable pointers
	//for easy saving
	virtual vector<Savable*> GetContainerContents(void * container, ReflectionData::SaveType containerType) = 0;
	//add an instance of this class to a container and return a pointer to the newly added data
	virtual Savable * AddInstanceToContainer(void * container, ReflectionData::SaveType containerType) = 0;
};
//handles container reflection 
//for help understanding review: http://stackoverflow.com/questions/4434569/is-it-possible-to-use-sfinae-templates-to-check-if-an-operator-exists
//controlled ambiguity is insane but necessary
class container_reflector {
public:
	//For comments regarding each function, see above
	virtual void ClearContainer(void * container, ReflectionData::SaveType containerType, int resizeCapacity) = 0;
	virtual vector<Savable*> GetContainerContents(void * container, ReflectionData::SaveType containerType) = 0;
	virtual Savable * AddInstanceToContainer(void * container, ReflectionData::SaveType containerType) = 0;
};

//An implementation of a reflector for any given savable class T
template <class ReflectedType>
class savablereflector : public reflector {
private:
	//This inner static class is used to handle all the container specific code
	//for each type of container
	template <template<class, class> class ContainerType, template<class> class AllocaterType>
	class utilities {
	public:
		typedef ContainerType<ReflectedType,AllocaterType<ReflectedType>> containertype;
		//Clear the container, and resize its internal array to be the given size
		static void clearContainer(void * container, int resizeCapacity) {
			containertype * correctContainer = (containertype*)container;
			correctContainer->clear();
			if (resizeCapacity > 0)
				correctContainer->resize(resizeCapacity);
		}
		//retrieve all the container's contents as a savable array
		static vector<Savable*> getContainerContents(void * container) {
			containertype * correctContainer = (containertype*)container;
			//the destination vector
			vector<Savable*> savables;
			//Retrieve all the contents and place in the destination vector
			for (auto containerMember : *correctContainer)
				savables.push_back(&containerMember);
			return savables;
		}
		//add an instance of this class to a container and return a pointer to the newly added data
		static Savable* addInstanceToContainer(void * container) {
			containertype * correctContainer = (containertype*)container;
			correctContainer->push_back(ReflectedType());
			return &correctContainer->back();
		}
	};

	//The reflector of the container if one exists
	container_reflector * containerData;
public:
	//The implementation of this template is never created except unless ReflectedType
	//has an assignment operator
	//that is also why it is a template-within-a-template even though its template parameter is the same
	template<class ReflectedTypeWithAssignmentOperator>
	class container_reflector_imp : public container_reflector {
	public:
		//Clear a container holding this type
		//and resize it to hold the given quantity of items
		void ClearContainer(void * container, ReflectionData::SaveType containerType, int resizeCapacity) override {
			//Work off of the given container
			switch(containerType) {
			case ReflectionData::SAVE_CONTIGOUSLIST:
				utilities<ContiguousList,__listdummyallocator>::clearContainer(container,resizeCapacity);
				break;
			case ReflectionData::SAVE_LIST:
				utilities<list,allocator>::clearContainer(container,resizeCapacity);
				break;
			case ReflectionData::SAVE_VECTOR:
				utilities<vector,allocator>::clearContainer(container,resizeCapacity);
				break;
			default:
				//Unsupported container
				_ASSERTE(false);
			}
		}
		//Convert a container of this type into a vector of savable pointers
		//for easy saving
		vector<Savable*> GetContainerContents(void * container, ReflectionData::SaveType containerType) override {
			//Work off of the given container
			switch(containerType) {
			case ReflectionData::SAVE_CONTIGOUSLIST:
				return utilities<ContiguousList,__listdummyallocator>::getContainerContents(container);
				break;
			case ReflectionData::SAVE_LIST:
				return utilities<list,allocator>::getContainerContents(container);
				break;
			case ReflectionData::SAVE_VECTOR:
				return utilities<vector,allocator>::getContainerContents(container);
				break;
			default:
				//Unsupported container
				_ASSERTE(false);
				return vector<Savable*>();
			}
		}
		//add an instance of this class to a container and return a pointer to the newly added data
		Savable * AddInstanceToContainer(void * container, ReflectionData::SaveType containerType) override {
			//Work off of the given container
			switch(containerType) {
			case ReflectionData::SAVE_CONTIGOUSLIST:
				return utilities<ContiguousList,__listdummyallocator>::addInstanceToContainer(container);
				break;
			case ReflectionData::SAVE_LIST:
				return utilities<list,allocator>::addInstanceToContainer(container);
				break;
			case ReflectionData::SAVE_VECTOR:
				return utilities<vector,allocator>::addInstanceToContainer(container);
				break;
			default:
				//Unsupported container
				_ASSERTE(false);
				return NULL;
			}
		}
	};


	//setup a reflector
	savablereflector(container_reflector * containerSupport) {
		//if the reflection supports containers, set them up here
		containerData = containerSupport;
	}
	~savablereflector() {
		delete containerData;
	}
	//Build an instance of the class
	Savable * BuildInstance() {
		return new ReflectedType();
	}
	//Clear a container holding this type
	//and resize it to hold the given quantity of items
	void ClearContainer(void * container, ReflectionData::SaveType containerType, int resizeCapacity) override {
		_ASSERTE(containerData != NULL); //You must call CLASS_SAVE_CONSTRUCTOR_CONTAINERS instead of CLASS_SAVE_CONSTRUCTOR if this assert hits
		containerData->ClearContainer(container,containerType,resizeCapacity);
	}
	//Convert a container of this type into a vector of savable pointers
	//for easy saving
	vector<Savable*> GetContainerContents(void * container, ReflectionData::SaveType containerType) override {
		_ASSERTE(containerData != NULL); //You must call CLASS_SAVE_CONSTRUCTOR_CONTAINERS instead of CLASS_SAVE_CONSTRUCTOR if this assert hits
		return containerData->GetContainerContents(container,containerType);
	}
	//add an instance of this class to a container and return a pointer to the newly added data
	Savable * AddInstanceToContainer(void * container, ReflectionData::SaveType containerType) override {
		_ASSERTE(containerData != NULL); //You must call CLASS_SAVE_CONSTRUCTOR_CONTAINERS instead of CLASS_SAVE_CONSTRUCTOR if this assert hits
		return containerData->AddInstanceToContainer(container,containerType);
	}
};

//A singleton class holding constructors
class ReflectionStore {
	//Map each class to its reflection data
	map<string,reflector*> reflectionStore;
public:
	static ReflectionStore & Data();

	//Register a class which can be constructed by the save system using a no-argument constructor
	void RegisterConstructableClass(string name, reflector* reflectionInformation);

	//Retrieve a class which can do anything which requires
	//direct knowledge of the type represented by the string name
	//such as constructing an instance, or dealing with container types
	reflector * RetrieveClassTypeTools(string name);
};