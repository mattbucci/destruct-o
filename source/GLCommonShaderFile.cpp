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
	string versionString = "";
	//Decide the version to force
	if (GLSLVersion == 140)
		versionString = "#version 140\n";
	//Add in all OS specifies
#ifdef WIN32
	virtualShader << "#define GLSL_WIN32\n";
#endif
#ifdef __APPLE__
	virtualShader << "#define GLSL_APPLE\n";
#endif
#ifdef __ANDROID__
	virtualShader << "#define GLSL_ANDROID\n";
#endif
#ifdef __MOBILE__
	virtualShader << "#define GLSL_MOBILE\n";
#endif
#ifdef __IPHONEOS__
	virtualShader << "#define GLSL_IPHONEOS\n";
#endif
	virtualShader << "#define GLSL_VERSION_" << GLSLVersion << "\n";
	//Now add the common file to the virtual shader
	virtualShader << string(&fileContents.front(),fileContents.size());
	//Now shove it all into a string and then a vector
	string vs = virtualShader.str();
	//First create the vertex shader file
	string textVertexShader = versionString + "#define VERTEX_SHADER\n" + vs;
	virtualShaderFileV.resize(textVertexShader.size());
	//Also count lines
	virtualShaderLength = 0;
	for (unsigned int i = 0; i < textVertexShader.size(); i++) {
		virtualShaderFileV[i] = textVertexShader[i];
		if (textVertexShader[i] == '\n')
			virtualShaderLength++;
	}
	//all done
	cout << "Built vertex common shader with " << virtualShaderLength << " lines of text.\n";
	//Next build the fragment shader file
	string textFragmentShader = versionString + "#define FRAGMENT_SHADER\n" + vs;
	virtualShaderFileF.resize(textFragmentShader.size());
	//Also count lines
	virtualShaderLength = 0;
	for (unsigned int i = 0; i < textFragmentShader.size(); i++) {
		virtualShaderFileF[i] = textFragmentShader[i];
		if (textFragmentShader[i] == '\n')
			virtualShaderLength++;
	}
	//all done
	cout << "Built fragment common shader with " << virtualShaderLength << " lines of text.\n";
}
GLCommonShaderFile::~GLCommonShaderFile() {

}

//Get the line count of the virtual shader file
int GLCommonShaderFile::GetShaderLineCount() {
	return virtualShaderLength;
}

//Get the text of the virtual shader
vector<char> GLCommonShaderFile::GetFragmentShaderText() {
	return virtualShaderFileF;
}
//Get the text of the virtual shader
vector<char> GLCommonShaderFile::GetVertexShaderText() {
	return virtualShaderFileV;
}