#pragma once

class GLLight {
	//Light intensities: constant,linear,quadratic
	vec3 intensities;
	//Color: RGB
	vec3 color;
	//components: ambient,diffuse, specular
	vec4 components;
	//light source location
	vec3 location;
	//uniform ids
	int uniformLightIntensity;
	int uniformLightColor;
	int uniformLightComponents;
	int uniformLightCoords;
public:
	//Creates an invalid instance of GLLight
	GLLight();
	//Creates a fully usable instance of GLLight
	GLLight(int uniformLightIntensity, int uniformLightColor, int uniformLightComponents, int uniformLightCoords);

	//Reset this light to be a simple white light
	//defaults:
	//	intensity: 50 units
	//	color: 1,1,1
	//	components: 0,1,1
	//	location: 0,0,0
	void Reset();

	//Apply the lighting
	void Apply();

	//Creates a white light with the specified intensity.
	//constantDecrease decreases the light intensity (which is 1) by a constant value
	//linearDecrease decreases the light intensity by a linear value
	//quadDecrease decreases the light intensity by a quadratic value
	void SetIntensity(float constantDecrease, float linearDecrease, float quadDecrease);

	//unitSize is the number of units the light can reach
	void SetIntensity(float unitSize);

	//Set the color of the light, RGB 0-1
	void SetColor(vec3 color);

	//Set the intensities of the different components of the light to be 0-1, specularWidth is .1-60
	void SetComponents(float ambient, float diffuse, float specular, float specularWidth);

	//Set the location of the light source
	void SetLocation(vec3 coordinates);
};