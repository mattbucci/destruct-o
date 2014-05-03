#include "stdafx.h"
#include "ShaderGroup.h"
#include "GL3DProgram.h"
#include "GL2DProgram.h"
#include "MaterialProgram.h"
#include "GLParticleProgram.h"
#include "GLTerrainProgram.h"
#include "GLEffectProgram.h"
#include "GLCommonShaderFile.h"

ShaderGroup::ShaderGroup() {
	// Build the common shader
	GLCommonShaderFile commonShader("shaders/commonshader.glsl");
	
	// Build the dialog shader
    GL2DProgram * shaders2d = new GL2DProgram(&commonShader,"shaders/vsh_interface.glsl","shaders/fsh_interface.glsl");
    if (!shaders2d->Valid()) 
        cout << "Failed to build opengl program\n";
    AddShader(shaders2d,"2d");
    
    // Build the voxel shader
    GL3DProgram * shaders3d = new GL3DProgram(&commonShader,"shaders/vsh_3d.glsl","shaders/fsh_3d.glsl");
    if (!shaders3d->Valid()) 
        cout << "Failed to build opengl program\n";
    AddShader(shaders3d,"3d");
	
	// Build Particle Shader
	GLParticleProgram * shadersP = new GLParticleProgram(&commonShader,"shaders/vsh_particle.glsl","shaders/fsh_particle.glsl");
	if (!shadersP->Valid()) 
		cout << "Failed to build opengl program\n";
	AddShader(shadersP,"particles");

	// Build Terrain Shader
	GLTerrainProgram * shadersT = new GLTerrainProgram(&commonShader,"shaders/vsh_terrain.glsl","shaders/fsh_terrain.glsl");
	if (!shadersT->Valid()) 
		cout << "Failed to build opengl program\n";
	AddShader(shadersT,"terrain");
    
    // Build the model shader
    MaterialProgram * modelShader = new MaterialProgram(&commonShader,"shaders/vsh_model.glsl","shaders/fsh_model.glsl");
    if (!modelShader->Valid())
        cout << "Failed to build opengl program (model)\n";
    AddShader(modelShader,"model");

    // Build the skybox shader
    GL3DProgram * skyboxShader = new GL3DProgram(&commonShader,"shaders/vsh_skybox.glsl","shaders/fsh_skybox.glsl");
    if (!skyboxShader->Valid())
        cout << "Failed to build opengl program (skybox)\n";
    AddShader(skyboxShader,"skybox");
    
	// Build Effect Shader
	GLEffectProgram * shadersE = new GLEffectProgram(&commonShader,"shaders/vsh_effect.glsl","shaders/fsh_effect.glsl");
	if (!shadersE->Valid()) 
		cout << "Failed to build opengl program\n";
	AddShader(shadersE,"effects");
}

ShaderGroup::~ShaderGroup() {
	//Clean up all the shaders
	for (auto shader : shaders)
		delete shader.second;
}

void ShaderGroup::AddShader(GLProgram * shader, string name) {
	shaders[name] = shader;
}
GLProgram * ShaderGroup::GetShader(string name) {
	return shaders[name];
}