#include "stdafx.h"
#include "GLProgram.h"
#include "GLShader.h"

GLProgram::GLProgram(string vertexShaderPath, string fragmentShaderPath, string geometryShaderPath) {
	vertexShader = new GLShader(vertexShaderPath,GL_VERTEX_SHADER);
	fragmentShader = new GLShader(fragmentShaderPath,GL_FRAGMENT_SHADER);
#ifndef __MOBILE__
	if (geometryShaderPath.length() > 0) 
		geometryShader = new GLShader(geometryShaderPath,GL_GEOMETRY_SHADER);
	else
#endif
		geometryShader = NULL;

	programId = glCreateProgram();
	glAttachShader(programId,vertexShader->shaderId);
	glAttachShader(programId,fragmentShader->shaderId);
	if (geometryShader != NULL) 
		glAttachShader(programId,geometryShader->shaderId);


	glLinkProgram(programId);
	GLint linked;
	//Check if linking was successful
	glGetProgramiv ( programId, GL_LINK_STATUS, &linked );
	if ( !linked ) { 
		cout << "Shader Linker Error:\n";
		GLint size;
		glGetProgramiv (  programId, GL_INFO_LOG_LENGTH, &size );
		string msg; msg.resize(size);
		glGetProgramInfoLog (  programId, size, NULL, &msg[0] );
		cout << msg << "\n";

		//Cleanup
		glDeleteProgram(programId);
		programId = -1;
		return;
	}
	else {
		GLint size;
		glGetProgramiv (  programId, GL_INFO_LOG_LENGTH, &size );
		string msg; msg.resize(size);
		glGetProgramInfoLog (  programId, size, NULL, &msg[0] );
		if (msg.length() > 5)
			cout << "Shader warnings:\n" << msg << "\n";
	}
}
GLProgram::~GLProgram() {
	if (programId > 0) {
		glDeleteProgram(programId);
		programId = -1;
	}
}

void GLProgram::UseProgram(void) {
	_ASSERTE(Valid());
	glUseProgram(programId);
}

int GLProgram::GetId() {
	return programId;
}

bool GLProgram::Valid(void) {
	//Check that the vertex shaders compiled correctly
	return (vertexShader->Compiled() && fragmentShader->Compiled() && (programId > 0));
}

void GLProgram::SetUniform2f(vec2 val, string name) {
	GLint loc = glGetUniformLocation(programId,name.c_str());
	if (loc < 0) {
		cout << "Failed to find uniform variable " << name << "\n";
		return;
	}
	glUniform2fv(loc,1,(float*)&val);
}

void GLProgram::EnableTextureSlot(string uniformName,int slot) {
	GLuint t1Location = glGetUniformLocation(programId, uniformName.c_str());
	glUniform1i(t1Location, slot);
}