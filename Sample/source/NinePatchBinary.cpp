#include "stdafx.h"
#include "NinePatchBinary.h"
#include <fstream>

//Create an empty ninepatch
NinePatchBinary::NinePatchBinary() {
	valid = false;
}
//Build a ninepatch for saving
NinePatchBinary::NinePatchBinary(int32_t left, int32_t width, int32_t top, int32_t height) {
	valid = true;
	savedNinepatch.endianByte = 1;
	memcpy(savedNinepatch.fileSignature,"PTCH",4);
	savedNinepatch.height = height;
	savedNinepatch.width = width;
	savedNinepatch.left = left;
	savedNinepatch.top = top;
}

void NinePatchBinary::Save(string binaryName) {
	ofstream toSave(binaryName,ios::binary);
	toSave.write((char *)&savedNinepatch,sizeof(savedNinepatch));
	//File is automatically saved when toSave leaves scope at the end of this function
}

//Get a list of 22 texture coordinates representing the 22 coordinates in the order necessary to make the 9patch
//the texture coordinate should be multiplied by the desired width/height to get vertex coordinates
//a triangle strip
//that is:
//A  C  E  G  
//BN DL FJ H 
//O  MQ KS IU
//P  R  T  V
vector<vec2> NinePatchBinary::GetTextureCoordinates() {
	return coordinates;
}

bool NinePatchBinary::IsValid() {
	return valid;
}

//Only compile this section if this isn't the 9patch compiler program
#ifndef NINE_PATCH_COMPILER

#include "GL2DProgram.h"
#include "lodepng.h"

//Attempt to load a ninepatch from file
NinePatchBinary::NinePatchBinary(string filename) {
	vector<unsigned char> ninepatchData;
	lodepng::load_file(ninepatchData,filename);
	valid = false;
	//Check that the file opened
	if (ninepatchData.size() < sizeof(savedNinepatch)) {
		cout << "Failed to open 9patch: " << filename << "\n";
		return;
	}
	//Read in bytes until the struct is populated
	memcpy((char *)&savedNinepatch,ninepatchData.data(),sizeof(savedNinepatch));
	//Check file signature
	if (string(savedNinepatch.fileSignature,4) != "PTCH") {
		cout << "Corrupted 9patch: " << filename << "\n";
		return;
	}
	//Check if the endian-ness of this machine is different than the file save type
	if (savedNinepatch.endianByte != 1) {
		//Swap required
		SwapEndian(&savedNinepatch.height);
		SwapEndian(&savedNinepatch.left);
		SwapEndian(&savedNinepatch.top);
		SwapEndian(&savedNinepatch.width);
	}
	//else all done loading binary file. Now load the texture into opengl
	
	//Derive texture name from ninepatch file name
	if (filename.size() <= 6) {
		cout << "Invalid 9patch filename: " << filename << "\n";
		return;
	}

	string imageFilename = filename.substr(0,filename.size()-6) + ".cropped.png";
	unsigned int width;
	unsigned int height;
	vector<unsigned char> image;
	//Read the file
	unsigned int error = lodepng::decode(image,width,height,imageFilename);
	if (error) {
		cout << "Failed to load texture for 9patch: " << filename << "\n";
		return;
	}

	glGenTextures( 1, (GLuint*)&openglTextureId );
	glBindTexture(GL_TEXTURE_2D, openglTextureId);


	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width,
				height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
				&image[0] );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );	//GL_NEAREST FOR SPEED
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_NEAREST FOR SPEED
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//record image size
	imageSize = vec2(width,height);

	{
		//Build the texture coordinates
		//This part copied and modified from the control.cpp file
		//that's what I use "vat" instead of modifying coordiantes directly
		vector<vec2> vat(22);

		float top = (float)savedNinepatch.top/imageSize.y;
		float left = (float)savedNinepatch.left/imageSize.x;
		float width = (float)savedNinepatch.width/imageSize.x;
		float height = (float)savedNinepatch.height/imageSize.y;

		//Draw the textured nine patch
		//by resizing the texture coordinates
		//using the 9patch system
		//Coordinates arranged like this:
		//A = 0, B = 1, C = 2, etc.
		//A  C  E  G  
		//BN DL FJ H 
		//O  MQ KS IU
		//P  R  T  V
		vat[0] = vec2(0,0);
		vat[1] = vec2(0,top);
		vat[2] = vec2(left,0);
		vat[3] = vec2(left,top);
		vat[4] = vec2(left+width,0);
		vat[5] = vec2(left+width,top);
		vat[6] = vec2(1,0);
		vat[7] = vec2(1,top);
		vat[8] = vec2(1,top+height);
		vat[9] = vat[5];
		vat[10] = vec2(left+width,top+height);
		vat[11] = vat[3];
		vat[12] = vec2(left,top+height);
		vat[13] = vat[1];
		vat[14] = vec2(0,top+height);
		vat[15] = vec2(0,1);
		vat[16] = vat[12];
		vat[17] = vec2(left,1);
		vat[18] = vat[10];
		vat[19] = vec2(left+width,1);
		vat[20] = vat[8];
		vat[21] = vec2(1,1);

		coordinates = vat;
	}


	valid = true;
	cout << "Successfully loaded 9patch: " << filename << "\n";
}

float NinePatchBinary::MWidth() {
	return (float)savedNinepatch.width;
}
float NinePatchBinary::MHeight() {
	return (float)savedNinepatch.height;
}
float NinePatchBinary::MLeft() {
	return (float)savedNinepatch.left;
}
float NinePatchBinary::MTop() {
	return (float)savedNinepatch.top;
}

vec2 NinePatchBinary::TotalSize() {
	return imageSize;
}

void NinePatchBinary::Bind9Texture(GL2DProgram * shader) {
	if (!valid)
		return;

	shader->EnableTexture(true);
	shader->SetColor(vec4(1,1,1,1));
	glBindTexture(GL_TEXTURE_2D,openglTextureId);
}
#endif

