
#pragma once

#include "GLCamera.h"
#include "GLProgram.h"
#include "GLModel.h"
#include "GLLighting.h"
#include "MaxDistanceFog.h"
#include "GLCombinedMatrix.h"
#include "GLAcidTab.h"
#include "GLVoxelColors.h"

class GLTerrainProgram : public GLProgram
{
	// Vertex attributes for the terrain program
	GLint attributeIndexTexture;
	GLint attributeIndexVertex;
	GLint attributeIndexMaterial;

    // Modelview uniform
    GLint uniformMatrixModelView;
    GLint uniformMatrixModelViewProjection;

	GLVoxelColors colors;
public:
	GLTerrainProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath);

	void UseProgram(void) override;

	GLModel Model;
	GLCamera Camera;
	MaxDistanceFog Fog;
	GLAcidTab Acid;

	//All the attributes used for drawing terrain
	const GLint AttributeTexture();
	const GLint AttributeVertex();
	const GLint AttributeMaterial();
    
    // Apply combined matrices
    void Apply();
};