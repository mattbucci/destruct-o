#include "stdafx.h"
#include "GLShader.h"
#include "GLCommonShaderFile.h"
#include "OS.h"
#include <fstream>
#include "lodepng.h"


GLShader::GLShader(const GLShader & other) {
	//Doesn't do anything. Exists to stop use
}
GLShader & GLShader::operator=(const GLShader & other) {
	//Doesn't do anything. Exists to stop use
	return *this;
}

//Attempts to compile in constructor
GLShader::GLShader(GLCommonShaderFile * commonShader, string filename,GLenum shaderType) {
	vector<unsigned char> fileContents;
	lodepng::load_file(fileContents,filename);
	
	shaderId = -1;

	if (fileContents.size() <= 0) {
		cout << "Failed to open shader file \"" << filename << "\""; 
		return;
	}


	//Null terminate
	fileContents.push_back(0);

	//Build shader
	shaderId = glCreateShader(shaderType);
	//Copy the virtual shader
	vector<char> virtualShader;
	if (shaderType == GL_VERTEX_SHADER)
		virtualShader = commonShader->GetVertexShaderText();
	else
		virtualShader = commonShader->GetFragmentShaderText();
	//Tack the loaded shader to the end of the virtual shader
	virtualShader.insert(virtualShader.end(),fileContents.begin(),fileContents.end());

	//For debug purposes
	/*{
		//Find only the file part of the filename
		size_t firstChar = filename.size()-1;
		for (;firstChar > 0; firstChar--)
			if (filename[firstChar-1] == '/')
				break;
		//Now write the shader
		ofstream debugOutput(OS::SaveDirectory() + string("debug_") + filename.substr(firstChar),ios::binary);
		for (unsigned int i = 0; i < virtualShader.size(); i++)
			debugOutput << virtualShader[i];
	}*/

	char * cstring = &virtualShader.front();
	glShaderSource(shaderId,1,(const GLchar**)&cstring,NULL);

	if (shaderId <= 0) {
		cout << "Failed to parse shader file \"" << filename << "\""; 
		return;
	}

	glCompileShader(shaderId);
	//Query status of compile
	GLint compiled;

	glGetShaderiv ( shaderId, GL_COMPILE_STATUS, &compiled );

	//Retrieve shader messages no matter what
	GLint size;
	//Retrieve the error from the shader logs
	glGetShaderiv ( shaderId, GL_INFO_LOG_LENGTH, &size );
	string msg; msg.resize(size);
	glGetShaderInfoLog ( shaderId, size, NULL, &msg[0] );

	if ( !compiled )
	{ 
		cout << "[0] Failed to compile shader file \"" << filename << "\":\n";
		cout << msg << "\n";

		//Cleanup
		glDeleteShader(shaderId);
		shaderId = -1;
		return;
	}
	else if (msg.length() > 5) {
		cout << "[1] Compiled shader file \"" << filename << "\" with warnings:\n"; 
		cout << msg << "\n";
	}
	else
		cout << "[2] Compiled shader file \"" << filename << "\"\n"; 
}
GLShader::~GLShader() {
	if (shaderId > 0) {
		glDeleteShader(shaderId);
		shaderId = -1;
	}
}

//Deterime if compile was successful
bool GLShader::Compiled(void) {
	return shaderId >= 0;
}