#include "stdafx.h"
#include "GLShader.h"

#include <fstream>

GLShader::GLShader(const GLShader & other) {
	//Doesn't do anything. Exists to stop use
}
GLShader & GLShader::operator=(const GLShader & other) {
	//Doesn't do anything. Exists to stop use
	return *this;
}

//Attempts to compile in constructor
GLShader::GLShader(string filename,GLenum shaderType) {
	vector<char> fileContents;
	ifstream file(filename.c_str(),ios::binary);
	
	shaderId = -1;

	if (!file.is_open()) {
		cout << "Failed to open shader file \"" << filename << "\""; 
		return;
	}

	//Read all the contents
	while (!file.eof()) {
		char byte;
		file.get(byte);
		fileContents.push_back(byte);
	}

	//Null ternimate
	fileContents.push_back(0);

	//Build shader
	shaderId = glCreateShader(shaderType);
	char * cstring = &fileContents.front();
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
		cout << "Failed to compile shader file \"" << filename << "\":\n";
		cout << msg << "\n";

		//Cleanup
		glDeleteShader(shaderId);
		shaderId = -1;
		return;
	}
	else if (msg.length() > 5) {
		cout << "Compiled shader file \"" << filename << "\" with warnings:\n"; 
		cout << msg << "\n";
	}
	else
		cout << "Compiled shader file \"" << filename << "\"\n"; 
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