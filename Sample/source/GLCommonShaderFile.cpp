#include "stdafx.h"
#include "GLCommonShaderFile.h"
#include <sstream>

//Builds the shader file text
GLCommonShaderFile::GLCommonShaderFile(string path) {
	vector<char> fileContents;
	SDL_RWops *file = SDL_RWFromFile(path.c_str(), "r"); 
	
	if (!file) {
		cout << "Failed to find common shader at path: \"" << path << "\"\n";
		return;
	}

	//Read all the contents (byte by byte is slow, but shader files are small)
	char byte;
	while (SDL_RWread(file, &byte, 1, 1) > 0) {
		fileContents.push_back(byte);
	}

	SDL_RWclose(file); 


	//Build the rest of a shader using a stringstream
	stringstream virtualShader;
	//Decide the version to force
#ifndef __IPHONEOS__
	if (OpenglVersion == 31)
		virtualShader << "#version 140\n";
#endif
	//Add in all OS specifies
#ifdef WIN32
	virtualShader << "#define WIN32\n";
#endif
#ifdef __APPLE__
	virtualShader << "#define __APPLE__\n";
#endif
#ifdef __ANDROID__
	virtualShader << "#define __ANDROID__\n";
#endif
#ifdef __MOBILE__
	virtualShader << "#define __MOBILE__\n";
#endif
#ifdef __IPHONEOS__
	virtualShader << "#define __IPHONEOS__\n";
#endif
	//Now add the common file to the virtual shader
	virtualShader << string(&fileContents.front(),fileContents.size());
	//Now shove it all into a string and then a vector
	string vs = virtualShader.str();
	virtualShaderFile.resize(vs.size());
	//Also count lines
	virtualShaderLength = 0;
	for (unsigned int i = 0; i < vs.size(); i++) {
		virtualShaderFile[i] = vs[i];
		if (vs[i] == '\n')
			virtualShaderLength++;
	}
	//all done
	cout << "Built common shader with " << virtualShaderLength << " lines of text.\n";
}
GLCommonShaderFile::~GLCommonShaderFile() {

}

//Get the line count of the virtual shader file
int GLCommonShaderFile::GetShaderLineCount() {
	return virtualShaderLength;
}

//Get the text of the virtual shader
vector<char> GLCommonShaderFile::GetShaderText() {
	return virtualShaderFile;
}