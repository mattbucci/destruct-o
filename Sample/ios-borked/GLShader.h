
#include "stdafx.h"

class GLShader {
	//No copying
	GLShader(const GLShader & other);
	GLShader & operator=(const GLShader & other);

	int shaderId;
	friend class GLProgram;
public:
	//Attempts to compile in constructor
	GLShader(string filename,GLenum shaderType);
	~GLShader();

	//Deterime if compile was successful
	bool Compiled(void);
};