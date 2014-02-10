
#pragma once
#include "GLCamera.h"
#include "GLProgram.h"
#include "GLCombinedModel.h"
#include "GLLighting.h"
#include "MaxDistanceFog.h"
#include "GLCombinedMatrix.h"
//#include "GLMaterialManager.h"

class GL3DProgram : public GLProgram {

	//All the attributes used for drawing voxels
	//Not all are valid on all platforms
	
	GLint attributeIndexTexture;
	GLint attributeIndexVertex;
	GLint attributeIndexNormal;
	GLint attributeIndexPosition;


	GLCombinedMatrix cmatrix;
public:
	GL3DProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath);

	GLLighting Lights;
	GLCombinedModel Model;
	GLCamera Camera;
	MaxDistanceFog Fog;
	//I've elected for manual management of textures
	//since the material manager concept wasn't really doing anything for me
	//GLMaterialManager Materials;

	//All the attributes used for drawing voxels
	//Not all are valid on all platforms
	const GLint AttributePosition();
	const GLint AttributeTexture();
	const GLint AttributeVertex();
	const GLint AttributeNormal();
};