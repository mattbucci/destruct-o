#pragma once

#include "stdafx.h"

class GLCommonShaderFile {
	vector<char> virtualShaderFile;
	//The length in lines
	int virtualShaderLength;
public:
	//Builds the shader file text
	//FIrst create the virtual file and then append the file found at path
	GLCommonShaderFile(string path);
	~GLCommonShaderFile();

	//Get the line count of the virtual shader file
	int GetShaderLineCount();

	//Get the text of the virtual shader
	vector<char> GetShaderText();

};