#include "stdafx.h"
#include "GLShader.h"
#include "GLCommonShaderFile.h"


GLShader::GLShader(const GLShader & other) {
	//Doesn't do anything. Exists to stop use
}
GLShader & GLShader::operator=(const GLShader & other) {
	//Doesn't do anything. Exists to stop use
	return *this;
}

//Attempts to compile in constructor
GLShader::GLShader(GLCommonShaderFile * commonShader, string filename,GLenum shaderType) {
	vector<char> fileContents;
	SDL_RWops *file = SDL_RWFromFile(filename.c_str(), "r"); 
	
	shaderId = -1;

	if (!file) {
		cout << "Failed to open shader file \"" << filename << "\""; 
		return;
	}
	
	//Read all the contents (byte by byte is slow, but shader files are small)
	char byte;
	while (SDL_RWread(file, &byte, 1, 1) > 0) {
		fileContents.push_back(byte);
	}

	SDL_RWclose(file); 

	//Null terminate
	fileContents.push_back(0);

	//Build shader
	shaderId = glCreateShader(shaderType);
	//Copy the virtual shader
	vector<char> virtualShader = commonShader->GetShaderText();
	//Tack the loaded shader to the end of the virtual shader
	virtualShader.insert(virtualShader.end(),fileContents.begin(),fileContents.end());

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