
#pragma once

#include <stdint.h>
#include "stdafx.h"

class GL2DProgram;

class NinePatchBinary {
	struct saveFormat {
		char fileSignature[4];
		uint32_t endianByte;
		//The number of pixels from the left before the start of the middle section
		int32_t left; 
		//The width of the middle section
		int32_t width;
		//The umber of pixels from the top before the start of the middle section
		int32_t top;
		//the height of the middle section
		int32_t height;
	};

	template <class T>
	//Swap the endian of the specified value
	//slow but always works
	void SwapEndian(T * value) {
		T temp;
		uint8_t * from = (uint8_t*)value;
		uint8_t * to = (uint8_t*)&temp;
		//Swap the order of each byte and store into temp
		for (unsigned int i = 0; i < sizeof(T); i++)
			to[i] = from[sizeof(T)-i];
		//Now overwrite the original with temp
		*value = temp;
	}

	//The raw 9patch information
	saveFormat savedNinepatch;
	bool valid;
	//If this was loaded from a file, it also loaded the texture
	int openglTextureId;
	//Precomputed return value for GetTextureCoordinates()
	vector<vec2> coordinates;
	//Only available when loaded from a file
	vec2 imageSize;
public:
	//Create an empty ninepatch
	NinePatchBinary();
	//Attempt to load a ninepatch from file
	//this should be the file path ending in .
	NinePatchBinary(string filename);
	//Build a ninepatch for saving
	NinePatchBinary(int32_t left, int32_t width, int32_t top, int32_t height);

	//Save the binary to a specific path
	//must end in correct extension or it wont work properly
	void Save(string binaryName);

	//Get the size and position of the middle section
	float MWidth();
	float MHeight();
	float MLeft();
	float MTop();
	//Get the total width,height of the image
	vec2 TotalSize();

	//If the ninepatch loaded correctly this is true
	bool IsValid();

	//Get a list of 22 texture coordinates representing the 22 coordinates in the order necessary to make the 9patch
	//the texture coordinate should be multiplied by the desired width/height to get vertex coordinates
	//a triangle strip
	//that is:
	//A  C  E  G  
	//BN DL FJ H 
	//O  MQ KS IU
	//P  R  T  V
	vector<vec2> GetTextureCoordinates();

	//Get the texture name of the ninepatch
	void Bind9Texture(GL2DProgram * shader);
};