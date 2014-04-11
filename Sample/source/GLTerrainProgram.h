
#pragma once
#include "GLCamera.h"
#include "GLProgram.h"
#include "GLCombinedModel.h"
#include "GLLighting.h"
#include "MaxDistanceFog.h"
#include "GLCombinedMatrix.h"
#include "GLAcidTab.h"
//#include "GLMaterialManager.h"

class GLTerrainProgram : public GLProgram {

	//All the attributes used for drawing voxels
	//Not all are valid on all platforms
	
	GLint attributeIndexTexture;
	GLint attributeIndexVertex;
	GLint attributeIndexShading;


	GLCombinedMatrix cmatrix;
public:
	GLTerrainProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath);

	GLCombinedModel Model;
	GLCamera Camera;
	MaxDistanceFog Fog;
	GLAcidTab Acid;

	//All the attributes used for drawing terrain
	const GLint AttributeTexture();
	const GLint AttributeVertex();
	const GLint AttributeShading();
};