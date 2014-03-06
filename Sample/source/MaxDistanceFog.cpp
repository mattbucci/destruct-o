#include "stdafx.h"
#include "MaxDistanceFog.h"


MaxDistanceFog::MaxDistanceFog(GLint programId) {
	glFogDistance = glGetUniformLocation(programId,"maxDistance");
	glFogColor = glGetUniformLocation(programId,"fogColor");
	SetFogColor(vec4(1.0,0,1.0,1.0));
	SetFogDistance(25.0f);
}
MaxDistanceFog::~MaxDistanceFog() {

}

//Also set the clear color
//to create a matching effect
void MaxDistanceFog::SetFogColor(vec4 color) {
	fogColor = color;
	glUniform4f(glFogColor,color.x,color.y,color.z,color.w);
}
vec4 MaxDistanceFog::GetFogColor() {
	return fogColor;
}

//Set fog distance
//fog starts to appear 3/4ths into this distance
void MaxDistanceFog::SetFogDistance(float distance) {
	fogDistance = distance;
	glUniform1f(glFogDistance,distance);
}
float MaxDistanceFog::GetFogDistance() {
	return fogDistance;
}


