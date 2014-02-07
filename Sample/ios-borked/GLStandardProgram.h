
#pragma once

#include "GLCamera.h"
#include "GLProgram.h"
#include "GLModel.h"

class GLStandardProgram : public GLProgram {
public:
	GLStandardProgram(string vertexShader, string fragmentShader, string geometryShader = "");
	virtual ~GLStandardProgram();
	GLModel Model;
	GLCamera Camera;


};