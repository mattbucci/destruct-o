#include "stdafx.h"
#include "GLLight.h"

GLLight::GLLight() {
	this->uniformLightIntensity = -1;
	this->uniformLightComponents = -1;
	this->uniformLightColor = -1;
	this->uniformLightCoords = -1;
}

GLLight::GLLight(int uniformLightIntensity, int uniformLightColor, int uniformLightComponents, int uniformLightCoords) {
	this->uniformLightIntensity = uniformLightIntensity;
	this->uniformLightComponents = uniformLightComponents;
	this->uniformLightColor = uniformLightColor;
	this->uniformLightCoords = uniformLightCoords;
}

//Reset this light to be a simple white light
//defaults:
//	intensity: 50 units
//	color: 1,1,1
//	components: 0,1,1
void GLLight::Reset() {
	//SetIntensity(0,0,.002);
	SetIntensity(45);
	SetColor(vec3(1,1,1));
	SetComponents(1,1,1,1);
	SetLocation(vec3(1,1,1));
}

//Apply the lighting
void GLLight::Apply() {
	//If in debug make sure uniforms are declared
	_ASSERTE(uniformLightColor>= 0);
	_ASSERTE(uniformLightComponents >= 0);
	_ASSERTE(uniformLightIntensity >= 0);
	_ASSERTE(uniformLightCoords >= 0);

	//Send the lighting values to the gpu shader
	glUniform3f(uniformLightIntensity,intensities.x,intensities.y,intensities.z);
	glUniform3f(uniformLightColor,color.r,color.g,color.b);
	glUniform4f(uniformLightComponents,components.x,components.y,components.z,components.w);
	glUniform3f(uniformLightCoords,location.x,location.y,location.z);
}

//quad -(f*d)^2
//linear -(f*d)

//Creates a white light with the specified intensity.
//constantDecrease decreases the light intensity (which is 1) by a constant value
//linearDecrease decreases the light intensity by a linear value
//quadDecrease decreases the light intensity by a quadratic value
void GLLight::SetIntensity(float constantDecrease, float linearDecrease, float quadDecrease) {
	intensities = vec3(constantDecrease,linearDecrease,quadDecrease);
}

//unitSize is the number of units the light can reach
void GLLight::SetIntensity(float unitSize) {
	intensities = vec3(0,0,1.0f/(unitSize*unitSize));
}

//Set the color of the light, RGB 0-1
void GLLight::SetColor(vec3 color) {
	this->color = color;
}

//Set the intensities of the different components of the light to be 0-1
void GLLight::SetComponents(float ambient, float diffuse, float specular, float specularWidth) {
	this->components = vec4(ambient,diffuse,specular,max<float>(.1,specularWidth));
}

void GLLight::SetLocation(vec3 coordinates) {
	location = coordinates;
}