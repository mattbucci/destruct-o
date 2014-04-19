
#pragma once

#include "ReflectionData.h"
#include "LoadData.h"

class Savable {

	void SaveValue(ReflectionData::savable valueData,Json::Value & value);

	void LoadValue(ReflectionData::savable valueData,Json::Value & value, LoadData & loadData);
	
	template <template<class, class> class ContainerType, template<class> class AllocaterType, class InternalType>
	void LoadSpecificContainer(ReflectionData::savable valueData,Json::Value & value, LoadData & loadData) {
		//First get a pointer to the class in question
		ContainerType<InternalType,AllocaterType<InternalType>> & container =  *(ContainerType<InternalType,AllocaterType<InternalType>>*)valueData.member;
		//clear out any data already in the class
		container.clear();
		//Now load values into the class
		for (unsigned int i = 0; i < value.size(); i++) {

			//Push a new value into the container
			container.push_back(InternalType());

			ReflectionData::savable saveData;
			saveData.dataType = valueData.internalType;
			saveData.internalType = ReflectionData::SAVE_NOTHING;
			saveData.member = &container.back();
			//saveData.member = (void *) &container.back();
			saveData.memberName = ""; //No name since its part of an array
			//Load the value
			LoadValue(saveData, value[i],loadData);


		}
	}

	template <template<class, class> class ContainerType, template<class> class AllocaterType, class InternalType>
	void LoadSpecificOwnedHandleContainer(ReflectionData::savable valueData,Json::Value & value, LoadData & loadData) {
		//First get a pointer to the class in question
		ContainerType<InternalType,AllocaterType<InternalType>> & container =  *(ContainerType<InternalType,AllocaterType<InternalType>>*)valueData.member;
		//clear out any data already in the class
		//Owned handles must be deleted, they can't just be cleared
		//copy the container encase the members have auto-deregistration or something like that
		ContainerType<InternalType,AllocaterType<InternalType>> containerCopy = container;
		for (auto member : containerCopy)
			delete (Savable*)member;
		container.clear();
		//Now load values into the class
		for (unsigned int i = 0; i < value.size(); i++) {
			InternalType valueToLoad = NULL;

			ReflectionData::savable saveData;
			saveData.dataType = valueData.internalType;
			saveData.internalType = ReflectionData::SAVE_NOTHING;
			saveData.member = &valueToLoad;
			saveData.memberName = ""; //No name since its part of an array
			//Load the value
			LoadValue(saveData, value[i],loadData);

			//Push the value into the container, unless the handle autoregisters
			if (valueData.internalType != ReflectionData::SAVE_OWNEDHANDLEAR)
				container.push_back(valueToLoad);
		}
	}

	template <template<class, class> class ContainerType, template<class> class AllocaterType>
	void LoadContainerValue(ReflectionData::savable valueData,Json::Value & value, LoadData & loadData) {
		switch (valueData.internalType) {
		case ReflectionData::SAVE_VEC2:
			LoadSpecificContainer<ContainerType,AllocaterType,vec2>(valueData,value,loadData);
			break;
		case ReflectionData::SAVE_VEC3:
			LoadSpecificContainer<ContainerType,AllocaterType,vec3>(valueData,value,loadData);
			break;
		case ReflectionData::SAVE_VEC4:
			LoadSpecificContainer<ContainerType,AllocaterType,vec4>(valueData,value,loadData);
			break;
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
		case ReflectionData::SAVE_OWNEDHANDLEAR:
		case ReflectionData::SAVE_OWNEDHANDLE:
			LoadSpecificOwnedHandleContainer<ContainerType,AllocaterType,void*>(valueData,value,loadData);
			break;
		case ReflectionData::SAVE_HANDLE:
			LoadSpecificContainer<ContainerType,AllocaterType,void*>(valueData,value,loadData);
			break;
		default:
			//Illegal type in container
			//SAVE_USEROWNEDHANDLE
			//not supported in containers
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
			InternalType tempMember = member;
			ReflectionData::savable saveData;
			saveData.dataType = valueData.internalType;
			saveData.internalType = ReflectionData::SAVE_NOTHING;
			
			saveData.member = &tempMember;
			saveData.memberName = ""; //No name since its part of an array
			SaveValue(saveData, value[i]);
			i++;
		}
	}

	template <template<class, class> class ContainerType, template<class> class AllocaterType>
	//Selects the appropriate type argument and calls SaveSpecificContainer
	void SaveContainerValue(ReflectionData::savable valueData,Json::Value & value) {
		switch (valueData.internalType) {
		case ReflectionData::SAVE_VEC2:
			SaveSpecificContainer<ContainerType,AllocaterType,vec2>(valueData,value);
			break;
		case ReflectionData::SAVE_VEC3:
			SaveSpecificContainer<ContainerType,AllocaterType,vec3>(valueData,value);
			break;
		case ReflectionData::SAVE_VEC4:
			SaveSpecificContainer<ContainerType,AllocaterType,vec4>(valueData,value);
			break;
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
		case ReflectionData::SAVE_OWNEDHANDLEAR:
		case ReflectionData::SAVE_OWNEDHANDLE:
		case ReflectionData::SAVE_HANDLE:
			SaveSpecificContainer<ContainerType,AllocaterType,void*>(valueData,value);
			break;
		default:
			//Illegal type in container
			//SAVE_USEROWNEDHANDLE
			//not supported in containers
			_ASSERTE(false);
		}
	}


	//Handle the json deserialize after the file has already been decoded
	static void DeserializeJson(Json::Value & root, Savable * loadInto);
protected:
	//Called to save all the data in this class
	virtual void Save(Json::Value & parentValue);
	//Called to load all the data in this class
	virtual void Load(Json::Value & parentValue, LoadData & loadData);

	virtual void savesystem_reflect(vector<ReflectionData::savable> & reflectMembersInto) = 0;

public:
	//Serialize data
	static vector<unsigned char> Serialize(Savable * classToSerialize);

	//Deserialize data into a new class and return it
	static Savable * Deserialize(vector<unsigned char> serializedData);
	
	//Deserialize data into an existing class
	static void Deserialize(vector<unsigned char> serializedData, Savable * saveInto);


	virtual string Name() = 0;


	Savable(){}
	virtual ~Savable(){}
};

