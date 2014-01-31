
#pragma once
#include "GLCamera.h"
#include "GLProgram.h"
#include "GLModel.h"
#include "GLLighting.h"
//#include "GLMaterialManager.h"

class GL3DProgram : public GLProgram {
public:
	GL3DProgram(string vertexShaderPath, string fragmentShaderPath);

	GLLighting Lights;
	GLModel Model;
	GLCamera Camera;
	//I've elected for manual management of textures
	//since the material manager concept wasn't really doing anything for me
	//GLMaterialManager Materials;
};