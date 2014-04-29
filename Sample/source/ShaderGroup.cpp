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
	//Build the common shader
	GLCommonShaderFile commonShader("shaders/commonshader.glsl");
	//Load the shaders appropriate for the opengl version being used
	if (OpenglVersion == 31) {
		//Build the dialog shader
		GL2DProgram * shaders2d = new GL2DProgram(&commonShader,"shaders/glsl31/vsh_interface.glsl","shaders/glsl31/fsh_interface.glsl");
		if (!shaders2d->Valid()) 
			cout << "Failed to build opengl program\n";
		AddShader(shaders2d,"2d");
        
		//Build the voxel shader
		GL3DProgram * shaders3d = new GL3DProgram(&commonShader,"shaders/glsl31/vsh_3d.glsl","shaders/glsl31/fsh_3d.glsl");
		if (!shaders3d->Valid()) 
			cout << "Failed to build opengl program\n";
		AddShader(shaders3d,"3d");
	}
	else {
		//Build the dialog shader
		GL2DProgram * shaders2d = new GL2DProgram(&commonShader,"shaders/glsl11/vsh_interface.glsl","shaders/glsl11/fsh_interface.glsl");
		if (!shaders2d->Valid()) 
			cout << "Failed to build opengl program\n";
		AddShader(shaders2d,"2d");
        
		//Build the voxel shader
		GL3DProgram * shaders3d = new GL3DProgram(&commonShader,"shaders/glsl11/vsh_3d.glsl","shaders/glsl11/fsh_3d.glsl");
		if (!shaders3d->Valid()) 
			cout << "Failed to build opengl program\n";
		AddShader(shaders3d,"3d");
	}
    
	//Build the dialog shader
	//Build Universal Shaders
	//Build Particle Shader
	GLParticleProgram * shadersP = new GLParticleProgram(&commonShader,"shaders/universal/vsh_particle.glsl","shaders/universal/fsh_particle.glsl");
	if (!shadersP->Valid()) 
		cout << "Failed to build opengl program\n";
	AddShader(shadersP,"particles");

	//Build Terrain Shader
	GLTerrainProgram * shadersT = new GLTerrainProgram(&commonShader,"shaders/universal/vsh_terrain.glsl","shaders/universal/fsh_terrain.glsl");
	if (!shadersT->Valid()) 
		cout << "Failed to build opengl program\n";
	AddShader(shadersT,"terrain");
    
    // Build the model shader
    MaterialProgram * modelShader = new MaterialProgram(&commonShader,"shaders/universal/vsh_model.glsl","shaders/universal/fsh_model.glsl");
    if (!modelShader->Valid())
        cout << "Failed to build opengl program (model)\n";
    AddShader(modelShader,"model");

    // Build the skybox shader
    GL3DProgram * skyboxShader = new GL3DProgram(&commonShader,"shaders/universal/vsh_skybox.glsl","shaders/universal/fsh_skybox.glsl");
    if (!skyboxShader->Valid())
        cout << "Failed to build opengl program (skybox)\n";
    AddShader(skyboxShader,"skybox");
    
	//Build Effect Shader
	GLEffectProgram * shadersE = new GLEffectProgram(&commonShader,"shaders/universal/vsh_effect.glsl","shaders/universal/fsh_effect.glsl");
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