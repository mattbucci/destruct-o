#include "stdafx.h"
#include "Savable.h"
#include "ContiguousList.h"
#include "lodepng.h"

#include "ReflectionData.h"
#include "ReflectionStore.h"
#include "LoadData.h"

void Savable::SaveValue(ReflectionData::savable valueData,Json::Value & value) {
	switch (valueData.dataType) {
	case ReflectionData::SAVE_VEC2I:
		value["x"] = ((vec2i*)valueData.member)->x;
		value["y"] = ((vec2i*)valueData.member)->y;
		return;
	case ReflectionData::SAVE_VEC2:
		value["x"] = ((vec2*)valueData.member)->x;
		value["y"] = ((vec2*)valueData.member)->y;
		return;
	case ReflectionData::SAVE_VEC3:
		value["x"] = ((vec3*)valueData.member)->x;
		value["y"] = ((vec3*)valueData.member)->y;
		value["z"] = ((vec3*)valueData.member)->z;
		return;
	case ReflectionData::SAVE_VEC4:
		value["x"] = ((vec4*)valueData.member)->x;
		value["y"] = ((vec4*)valueData.member)->y;
		value["z"] = ((vec4*)valueData.member)->z;
		value["w"] = ((vec4*)valueData.member)->w;
		return;
	case ReflectionData::SAVE_BOOL:
		value = *(bool*)valueData.member;
		return;
	case ReflectionData::SAVE_INT8:
		value = *(int8_t*)valueData.member;
		return;
	case ReflectionData::SAVE_UINT8:
		value = *(uint8_t*)valueData.member;
		return;
	case ReflectionData::SAVE_INT16:
		value = *(int16_t*)valueData.member;
		return;
	case ReflectionData::SAVE_UINT16:
		value = *(uint16_t*)valueData.member;
		return;
	case ReflectionData::SAVE_INT32:
		value = *(int32_t*)valueData.member;
		return;
	case ReflectionData::SAVE_UINT32:
		value = *(uint32_t*)valueData.member;
		return;
	case ReflectionData::SAVE_INT64:
		value = *(int64_t*)valueData.member;
		return;
	case ReflectionData::SAVE_UINT64:
		value = *(uint64_t*)valueData.member;
		return;
	case ReflectionData::SAVE_FLOAT:
		value = *(float*)valueData.member;
		return;
	case ReflectionData::SAVE_DOUBLE:
		value = *(double*)valueData.member;
		return;
	case ReflectionData::SAVE_STRING:
		value = *(string*)valueData.member;
		return;
	//Saves the handle: but does not save what the handle points to
	case ReflectionData::SAVE_HANDLE:
		if (sizeof(void*) == 4)
			//32 bit
			//Everything stored as 64 bit integers
			value = (uint64_t)*(uint32_t*)valueData.member;
		else
			//Assume 64 bit
			value = *(uint64_t*)valueData.member;
		return;

	//Saves the handle: and saves what it points to
	//will reconstruct the object automatically upon load
	case ReflectionData::SAVE_USEROWNEDHANDLE:
	case ReflectionData::SAVE_OWNEDHANDLEAR:
	case ReflectionData::SAVE_OWNEDHANDLE: {
		//Retrieve the pointer value
		uint64_t handle;
		if (sizeof(void*) == 4)
			//32 bit
			//Everything stored as 64 bit integers
			handle = (uint64_t)*(uint32_t*)valueData.member;
		else
			//Assume 64 bit
			handle = *(uint64_t*)valueData.member;
		//Save the new class with some internal information about its original handle
		value["__HANDLE__"] = handle;
		value["__TYPE__"] = (*(Savable**)valueData.member)->Name();
		(*(Savable**)valueData.member)->Save(value);
		return;
	}
	case ReflectionData::SAVE_INSTANCE: {
		//Retrieve the pointer value
		uint64_t handle;
		if (sizeof(void*) == 4)
			//32 bit
			//Everything stored as 64 bit integers
			handle = (uint64_t) valueData.member;
		else
			//Assume 64 bit
			handle = (uint64_t)valueData.member;
		//Save the new class with some internal information about its original handle
		value["__HANDLE__"] = handle;
		value["__TYPE__"] = ((Savable*)valueData.member)->Name();
		((Savable*)valueData.member)->Save(value);
		return;
	}

	case ReflectionData::SAVE_LIST:
		SaveContainerValue<list,allocator>(valueData,value);
		return;
	case ReflectionData::SAVE_VECTOR:
		SaveContainerValue<vector,allocator>(valueData,value);
		return;
	case ReflectionData::SAVE_CONTIGOUSLIST:
		SaveContainerValue<ContiguousList,__listdummyallocator>(valueData,value);
		return;
	default:
		_ASSERTE(false);
	}
}

void Savable::LoadValue(ReflectionData::savable valueData,Json::Value & value, LoadData & loadData) {
	switch (valueData.dataType) {
	case ReflectionData::SAVE_VEC2I:
		((vec2i*)valueData.member)->x = value["x"].asInt();
		((vec2i*)valueData.member)->y = value["y"].asInt();
		return;
	case ReflectionData::SAVE_VEC2:
		((vec2*)valueData.member)->x = value["x"].asFloat();
		((vec2*)valueData.member)->y = value["y"].asFloat();
		return;
	case ReflectionData::SAVE_VEC3:
		((vec3*)valueData.member)->x = value["x"].asFloat();
		((vec3*)valueData.member)->y = value["y"].asFloat();
		((vec3*)valueData.member)->z = value["z"].asFloat();
		return;
	case ReflectionData::SAVE_VEC4:
		((vec4*)valueData.member)->x = value["x"].asFloat();
		((vec4*)valueData.member)->y = value["y"].asFloat();
		((vec4*)valueData.member)->z = value["z"].asFloat();
		((vec4*)valueData.member)->w = value["w"].asFloat();
		return;
	case ReflectionData::SAVE_BOOL:
		*(bool*)valueData.member = (bool)value.asBool();
		return;
	case ReflectionData::SAVE_INT8:
		*(int8_t*)valueData.member = (int8_t)value.asInt();
		return;
	case ReflectionData::SAVE_UINT8:
		*(uint8_t*)valueData.member = (uint8_t)value.asUInt();
		return;
	case ReflectionData::SAVE_INT16:
		*(int16_t*)valueData.member = (int16_t)value.asInt();
		return;
	case ReflectionData::SAVE_UINT16:
		*(uint16_t*)valueData.member = (uint16_t)value.asUInt();
		return;
	case ReflectionData::SAVE_INT32:
		*(int32_t*)valueData.member = (int32_t)value.asInt();
		return;
	case ReflectionData::SAVE_UINT32:
		*(uint32_t*)valueData.member = (uint32_t)value.asUInt();
		return;
	case ReflectionData::SAVE_INT64:
		*(int64_t*)valueData.member = (int64_t)value.asInt64();
		return;
	case ReflectionData::SAVE_UINT64:
		*(uint64_t*)valueData.member = (uint64_t)value.asUInt64();
		return;
	case ReflectionData::SAVE_FLOAT:
		*(float*)valueData.member = (float)value.asFloat();
		return;
	case ReflectionData::SAVE_DOUBLE:
		*(double*)valueData.member = (double)value.asDouble();
		return;
	case ReflectionData::SAVE_STRING:
		*(string*)valueData.member = value.asString();
		return;
	//Saves the handle: but does not save what the handle points to
	case ReflectionData::SAVE_HANDLE: {
		uint64_t loadedValue;
		loadedValue = value.asUInt64();
		if (loadedValue != 0)
			//Register it to be connected to an actual class
			loadData.RegisterHandleToLoad(loadedValue,(void**)valueData.member);
		else
			//Just set it NULL
			*(void**)valueData.member = nullptr;
		return;
	}
	case ReflectionData::SAVE_USEROWNEDHANDLE: {
		uint64_t loadedValue = value["__HANDLE__"].asUInt64();

		//Assume the user has told you where you can find the given object
		//if not, then the user is missing a REPAIR_HANDLE(variable) in the load routine
		Savable * loadedInstance = (Savable*)loadData.RetrieveConstructedHandle(loadedValue);
		//Load the object
		loadedInstance->Load(value,loadData);

		return;
	}

	//Saves the handle: and saves what it points to
	//will reconstruct the object automatically upon load
	case ReflectionData::SAVE_OWNEDHANDLEAR:
	case ReflectionData::SAVE_OWNEDHANDLE: {
		uint64_t loadedValue;
		loadedValue = value["__HANDLE__"].asUInt64();
		if (loadedValue != 0) {
			Savable * classInstance = ReflectionStore::Data().RetrieveClassInstance(value["__TYPE__"].asString());
			//Save the fact you constructed a class so that anything else
			//that was supposed to have a handle to that class can have their handles reconstructed
			loadData.RegisterLoadedHandle(loadedValue,classInstance);

			//Delete the previous instance of whatever was owned
			delete *(Savable**)valueData.member;

			if (valueData.dataType != ReflectionData::SAVE_OWNEDHANDLEAR)
				//Unless, this handle autoregisters, set it to the new instance
				*(void**)valueData.member = (void*)classInstance;

			//Load the class
			classInstance->Load(value,loadData);
		}
		else
			//Just set it NULL
			*(void**)valueData.member = nullptr;
		return;
	}

	case ReflectionData::SAVE_INSTANCE: {
		uint64_t loadedValue;
		loadedValue = value["__HANDLE__"].asUInt64();
		if (loadedValue != 0) {
			//Save the fact you constructed a class so that anything else
			//that was supposed to have a handle to that class can have their handles reconstructed
			loadData.RegisterLoadedHandle(loadedValue,(Savable*)valueData.member);
			//Load the class
			((Savable*)valueData.member)->Load(value,loadData);
		}
		else {
			cout << "Handle was NULL on SAVE_INSTANCE...\n";
			_ASSERTE(false);
		}
		return;
	}
	case ReflectionData::SAVE_LIST:
		LoadContainerValue<list,allocator>(valueData,value,loadData);
		return;
	case ReflectionData::SAVE_VECTOR:
		LoadContainerValue<vector,allocator>(valueData,value,loadData);
		return;
	case ReflectionData::SAVE_CONTIGOUSLIST:
		LoadContainerValue<ContiguousList,__listdummyallocator>(valueData,value,loadData);
		return;
	default:
		_ASSERTE(false);
	}
}



void Savable::Save(Json::Value & parentValue) {
	//load reflection information for this class
	vector<ReflectionData::savable> members;
	savesystem_reflect(members);
	//save every member of the class
	for (auto member : members)
		SaveValue(member,parentValue[member.memberName]);
}

void Savable::Load(Json::Value & parentValue, LoadData & loadData) {
	//load reflection information for this class
	vector<ReflectionData::savable> members;
	savesystem_reflect(members);
	//Load every member of the class
	for (auto member : members) {
		if (member.memberName == "cityPopulation")
			cout << "";
		if (!parentValue.isMember(member.memberName))
			cout << "Member: " << member.memberName << " missing.\n";
		else
			LoadValue(member,parentValue[member.memberName],loadData);
	}
}


//zlib functions borrowed from lodepng
//already embedded in our program so we might as well reuse them
static vector<unsigned char> deflateArray(vector<unsigned char> toDeflate) {
	vector<unsigned char> buffer;
	//lodepng requires a c style buffer
	//it automatically resizes the buffer to the desired size
	unsigned char * cbuffer = (unsigned char *)malloc(1);

	LodePNGCompressSettings compressSettings;
	compressSettings.btype = 2;
	compressSettings.use_lz77 = 1;
	compressSettings.windowsize = 2048;
	compressSettings.minmatch = 3;
	compressSettings.nicematch = 128;
	compressSettings.lazymatching = true;
	compressSettings.custom_context = NULL;
	compressSettings.custom_zlib = NULL;
	compressSettings.custom_deflate = NULL;

	size_t outsize = 0;
	lodepng_zlib_compress(&cbuffer,&outsize,&toDeflate.front(),toDeflate.size(),&compressSettings);
	
	//Now copy to a vector buffer
	buffer.resize(outsize);
	for (unsigned int i = 0; i < outsize; i++)
		buffer[i] = cbuffer[i];

	//free the cbuffer
	free(cbuffer);

	return buffer;
}

static vector<unsigned char> inflateArray(vector<unsigned char> toInflate) {
	vector<unsigned char> buffer;
	//lodepng requires a c style buffer
	//it automatically resizes the buffer to the desired size
	unsigned char * cbuffer = (unsigned char *)malloc(1);

	LodePNGDecompressSettings decompressSettings;
	decompressSettings.ignore_adler32 = 0;
	decompressSettings.custom_context = NULL;
	decompressSettings.custom_inflate = NULL;
	decompressSettings.custom_zlib = NULL;


	size_t outsize = 0;
	lodepng_zlib_decompress(&cbuffer,&outsize,&toInflate.front(),toInflate.size(),&decompressSettings);
	
	//Now copy to a vector buffer
	buffer.resize(outsize);
	for (unsigned int i = 0; i < outsize; i++)
		buffer[i] = cbuffer[i];

	//free the cbuffer
	free(cbuffer);

	return buffer;
}



//Serialize data
vector<unsigned char> Savable::Serialize(Savable * classToSerialize, bool compressed) {
	Json::Value root;
	//Serialize the class into the root value
	classToSerialize->Save(root);
	//Add the type as a special root type value
	root["__ROOTTYPE__"] = classToSerialize->Name();
	//For now use a styled writer so we can look at pretty json
	Json::StyledWriter writer;
	string json = writer.write(root);
	//Copy to a vector
	vector<unsigned char> rawData(json.size());
	for (unsigned int i = 0; i < json.size();i++)
		rawData[i] = json[i];

	//Deflate the vector
	if (compressed)
		rawData = deflateArray(rawData);


	return rawData;
}

void Savable::DeserializeJson(Json::Value & root, Savable * loadInto) {
	LoadData load;
	//Load the object form the json
	loadInto->Load(root,load);
	//Rebuild pointers registered in load data
	load.FinishLoading();
}

//Deserialize data into a new class and return it
Savable * Savable::Deserialize(vector<unsigned char> serializedData, bool compressed) {
	Json::Reader reader;
	Json::Value root;

	//Compress data
	if (compressed)
		serializedData = inflateArray(serializedData);

	//parse the json object 
	reader.parse(string((char*)&serializedData[0],serializedData.size()),root);
	//create a new object for the serialized data
	Savable * createdObject = ReflectionStore::Data().RetrieveClassInstance(root["__ROOTTYPE__"].asString());
	//Load all the data into the new object
	DeserializeJson(root,createdObject);

	return createdObject;
}
	
//Deserialize data into an existing class
void Savable::Deserialize(vector<unsigned char> serializedData, Savable * saveInto, bool compressed) {
	Json::Reader reader;
	Json::Value root;

	//Decompress data
	if (compressed)
		serializedData = inflateArray(serializedData);

	//parse the json object 
	reader.parse(string((char*)&serializedData[0],serializedData.size()),root);
	//Load all the data into the new object
	DeserializeJson(root,saveInto);
}