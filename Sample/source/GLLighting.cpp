#include "stdafx.h"
#include "GLLighting.h"

GLLighting::GLLighting(GLuint programId) {
	//Build the light objects
	a = GLLight(glGetUniformLocation(programId,"light1_intensity"),
				glGetUniformLocation(programId,"light1_color"),
				glGetUniformLocation(programId,"light1_components"),
				glGetUniformLocation(programId,"light1_position"));

	b = GLLight(glGetUniformLocation(programId,"light2_intensity"),
				glGetUniformLocation(programId,"light2_color"),
				glGetUniformLocation(programId,"light2_components"),
				glGetUniformLocation(programId,"light2_position"));

	c = GLLight(glGetUniformLocation(programId,"light3_intensity"),
				glGetUniformLocation(programId,"light3_color"),
				glGetUniformLocation(programId,"light3_components"),
				glGetUniformLocation(programId,"light3_position"));

	uniformLightCount = glGetUniformLocation(programId,"light_count");
	Reset();
}
GLLighting::~GLLighting() {

}

//Reset all three lighting effects, disable all three
void GLLighting::Reset() {
	a.Reset();
	b.Reset();
	c.Reset();
	activeLights = 0;
}

//Enable the total number of lights specified. Supported 0-3 inclusive
void GLLighting::Enable(int lightCount) {
	activeLights = lightCount;
	lastActivatedLightCount = activeLights;
}

//Apply current lighting effects to the shaders
void GLLighting::Apply() {
	//Make sure the light count is a valid uniform
	//_ASSERTE(uniformLightCount >= 0);

	//Apply the lights marked as active
	if (activeLights > 0)
		a.Apply();
	if (activeLights > 1)
		b.Apply();
	if (activeLights > 2)
		c.Apply();

	//Send the number of lights
	glUniform1i(uniformLightCount,activeLights);
}

//Turn off all previously enabled lighting effects
//acts the same as Reset(); Apply();
void GLLighting::Disable() {
	Reset();
	Apply();
}

//Turn off lights, but perserve information
//so that On() restores them as they were
void GLLighting::Off() {
	activeLights = 0;
}
//Turn on lights that were previously turned off
//Enable() can also be used
void GLLighting::On() {
	activeLights = lastActivatedLightCount;
}

//Select a light to modify its properties
GLLight & GLLighting::operator[](const int lightId) {
	//Make sure light index is valid
	_ASSERTE((lightId >= 0) && (lightId <= 2));
	//Return the specified light
	switch (lightId) {
	case 0:
		return a;
	case 1:
		return b;
	case 2:
		return c;
	}
	//Shouldn't happen
	return c;
}