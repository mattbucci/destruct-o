#include "stdafx.h"
#include "ReflectionData.h"
#include "ContiguousList.h"
#include "lodepng.h"

//Register a recreated object
void LoadData::RegisterLoadedHandle(uint64_t original, void * newHandle) {
	//If the handle is being rebuilt twice, we have a problem
	_ASSERTE(handlesRebuilt.find(original) == handlesRebuilt.end());
	handlesRebuilt[original] = newHandle;
}
//Register a pointer to a recreated object which has not been created yet
void LoadData::RegisterHandleToLoad(uint64_t original, void ** handleToLoad) {
	loadhandle lh;
	lh.handleToLoad = handleToLoad;
	lh.originalHandle = original;
	handlesToLoad.push_back(lh);
}

//Use the list of built handles and the
//originalHandles to match created objects
//to old pointers to created objects
//and patch up the pointers to point to the new objects
void LoadData::FinishLoading() {
	for (auto handleToPatch : handlesToLoad) {
		//Verify the handle was built and registered
		//If no object was built you can't fix the handle, and there's a problem!
		_ASSERTE(handlesRebuilt.find(handleToPatch.originalHandle) != handlesRebuilt.end());

		//Restore the pointer to a new valid value
		*handleToPatch.handleToLoad = handlesRebuilt[handleToPatch.originalHandle];
	}
}

void Savable::SaveValue(ReflectionData::savable valueData,Json::Value & value) {
	switch (valueData.dataType) {
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
			handle = (uint32_t)valueData.member;
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
	case ReflectionData::SAVE_BOOL:
		*(bool*)valueData.member = (bool)value.asInt();
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

	//Saves the handle: and saves what it points to
	//will reconstruct the object automatically upon load
	case ReflectionData::SAVE_OWNEDHANDLE: {
		uint64_t loadedValue;
		loadedValue = value["__HANDLE__"].asUInt64();
		if (loadedValue != 0) {
			Savable * classInstance = ReflectionStore::Data().RetrieveClassInstance(value["__TYPE__"].asString());
			//Save the fact you constructed a class so that anything else
			//that was supposed to have a handle to that class can have their handles reconstructed
			loadData.RegisterLoadedHandle(loadedValue,classInstance);
			//Set it to the new instance
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
		LoadContainerValue<ContiguousList,allocator>(valueData,value,loadData);
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
		if (!parentValue.isMember(member.memberName))
			cout << "Member: " << member.memberName << " missing.\n";
		else
			LoadValue(member,parentValue[member.memberName],loadData);
	}
}


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

/*
Compresses data with Zlib. Reallocates the out buffer and appends the data.
Zlib adds a small header and trailer around the deflate data.
The data is output in the format of the zlib specification.
Either, *out must be NULL and *outsize must be 0, or, *out must be a valid
buffer and *outsize its size in bytes. out must be freed by user after usage.

unsigned lodepng_zlib_decompress(unsigned char** out, size_t* outsize,
                                 const unsigned char* in, size_t insize,
                                 const LodePNGDecompressSettings* settings);




unsigned lodepng_zlib_compress(unsigned char** out, size_t* outsize,
                               const unsigned char* in, size_t insize,
                               const LodePNGCompressSettings* settings);

*/

  /*LZ77 related settings*/
  unsigned btype; /*the block type for LZ (0, 1, 2 or 3, see zlib standard). Should be 2 for proper compression.*/
  unsigned use_lz77; /*whether or not to use LZ77. Should be 1 for proper compression.*/
  unsigned windowsize; /*the maximum is 32768, higher gives more compression but is slower. Typical value: 2048.*/
  unsigned minmatch; /*mininum lz77 length. 3 is normally best, 6 can be better for some PNGs. Default: 0*/
  unsigned nicematch; /*stop searching if >= this length found. Set to 258 for best compression. Default: 128*/
  unsigned lazymatching; /*use lazy matching: better compression but a bit slower. Default: true*/

  /*use custom zlib encoder instead of built in one (default: null)*/
  unsigned (*custom_zlib)(unsigned char**, size_t*,
                          const unsigned char*, size_t,
                          const LodePNGCompressSettings*);
  /*use custom deflate encoder instead of built in one (default: null)
  if custom_zlib is used, custom_deflate is ignored since only the built in
  zlib function will call custom_deflate*/
  unsigned (*custom_deflate)(unsigned char**, size_t*,
                             const unsigned char*, size_t,
                             const LodePNGCompressSettings*);

  void* custom_context; /*optional custom settings for custom functions*/


//zlib functions borrowed from lodepng
//already embedded in our program so we might as well reuse them
vector<unsigned char> deflateArray(vector<unsigned char> toDeflate) {
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
	for (int i = 0; i < outsize; i++)
		buffer[i] = cbuffer[i];

	//free the cbuffer
	free(cbuffer);

	return buffer;
}

vector<unsigned char> inflateArray(vector<unsigned char> toInflate) {
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
	for (int i = 0; i < outsize; i++)
		buffer[i] = cbuffer[i];

	//free the cbuffer
	free(cbuffer);

	return buffer;
}



//Serialize data
vector<unsigned char> Savable::Serialize(Savable * classToSerialize) {
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
	for (int i = 0; i < json.size();i++)
		rawData[i] = json[i];

	//Deflate the vector
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
Savable * Savable::Deserialize(vector<unsigned char> serializedData) {
	Json::Reader reader;
	Json::Value root;

	//Decompress data
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
void Savable::Deserialize(vector<unsigned char> serializedData, Savable * saveInto) {
	Json::Reader reader;
	Json::Value root;

	//Decompress data
	serializedData = inflateArray(serializedData);

	//parse the json object 
	reader.parse(string((char*)&serializedData[0],serializedData.size()),root);
	//Load all the data into the new object
	DeserializeJson(root,saveInto);
}