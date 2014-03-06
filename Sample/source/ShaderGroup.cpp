#include "stdafx.h"
#include "ShaderGroup.h"
#include "GL3DProgram.h"
#include "GL2DProgram.h"
#include "GLParticleProgram.h"
#include "GLCommonShaderFile.h"

ShaderGroup::ShaderGroup() {
	//Build the common shader
	GLCommonShaderFile commonShader("Interface/Shaders/commonshader.glsl");
	//Load the shaders appropriate for the opengl version being used
	if (OpenglVersion == 31) {
		//Build the dialog shader
		GL2DProgram * shaders2d = new GL2DProgram(&commonShader,"Interface/Shaders/glsl31/vsh_interface.glsl","Interface/Shaders/glsl31/fsh_interface.glsl");
		if (!shaders2d->Valid()) 
			cout << "Failed to build opengl program\n";
		AddShader(shaders2d,"2d");
        
		//Build the voxel shader
		GL3DProgram * shaders3d = new GL3DProgram(&commonShader,"Interface/Shaders/glsl31/vsh_3d.glsl","Interface/Shaders/glsl31/fsh_3d.glsl");
		if (!shaders3d->Valid()) 
			cout << "Failed to build opengl program\n";
		AddShader(shaders3d,"3d");
        
		//Build the mesh shader
		GL3DProgram * shadersMesh = new GL3DProgram(&commonShader,"Interface/Shaders/glsl31/vsh_mesh.glsl","Interface/Shaders/glsl31/fsh_mesh.glsl");
		if (!shadersMesh->Valid())
			cout << "Failed to build opengl program (mesh)\n";
		AddShader(shadersMesh,"mesh");
	}
	else {
		//Build the dialog shader
		GL2DProgram * shaders2d = new GL2DProgram(&commonShader,"Interface/Shaders/glsl11/vsh_interface.glsl","Interface/Shaders/glsl11/fsh_interface.glsl");
		if (!shaders2d->Valid()) 
			cout << "Failed to build opengl program\n";
		AddShader(shaders2d,"2d");
        
		//Build the voxel shader
		GL3DProgram * shaders3d = new GL3DProgram(&commonShader,"Interface/Shaders/glsl11/vsh_3d.glsl","Interface/Shaders/glsl11/fsh_3d.glsl");
		if (!shaders3d->Valid()) 
			cout << "Failed to build opengl program\n";
		AddShader(shaders3d,"3d");
        
		//Build the mesh shader
		GL3DProgram * shadersMesh = new GL3DProgram(&commonShader,"Interface/Shaders/glsl11/vsh_mesh.glsl","Interface/Shaders/glsl11/fsh_mesh.glsl");
		if (!shadersMesh->Valid())
			cout << "Failed to build opengl program (mesh)\n";
		AddShader(shadersMesh,"mesh");
	}
	//Build the dialog shader
	GLParticleProgram * shadersP = new GLParticleProgram(&commonShader,"Interface/Shaders/universal/vsh_particle.glsl","Interface/Shaders/universal/fsh_particle.glsl");
	if (!shadersP->Valid()) 
		cout << "Failed to build opengl program\n";
	AddShader(shadersP,"particles");
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