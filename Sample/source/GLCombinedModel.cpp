#include "stdafx.h"
#include "GLCombinedModel.h"
#include "GLCombinedMatrix.h"

GLCombinedModel::GLCombinedModel(GLCombinedMatrix * cmatrix) {
	this->cmatrix = cmatrix;
	Reset();
}

void GLCombinedModel::Reset() {
	stackPosition = 0;
	modelMatrix[0] = mat4();
}
void GLCombinedModel::Apply(){
	cmatrix->Model = modelMatrix[stackPosition];
	cmatrix->Apply();
}

mat4 GLCombinedModel::GetMatrix(void){
	return modelMatrix[stackPosition];
}

void GLCombinedModel::SetMatrix(const mat4 & matrix){
	modelMatrix[stackPosition] = matrix;
}


void GLCombinedModel::PushMatrix(void){
	_ASSERTE(stackPosition < 256);

	stackPosition++;
	modelMatrix[stackPosition] = modelMatrix[stackPosition-1];
}

void GLCombinedModel::PopMatrix(void){
	_ASSERTE(stackPosition > 0);

	stackPosition--;
}

vec3 GLCombinedModel::TransformVector(vec3 vertex) const {
	vec4 tv = modelMatrix[stackPosition] * vec4(vertex,1.0f);
	return vec3(tv.x,tv.y,tv.z);
}


GLCombinedModel & GLCombinedModel::Translate(vec3 translation){
	modelMatrix[stackPosition] *= glm::translate(translation);
	return *this;
}
GLCombinedModel & GLCombinedModel::Translate(float x, float y, float z){
	modelMatrix[stackPosition] *= glm::translate(vec3(x,y,z));
	return *this;
}
GLCombinedModel & GLCombinedModel::Rotate(float angle, vec3 axis){
	modelMatrix[stackPosition] *= glm::rotate(angle,axis);
	return *this;
}
GLCombinedModel & GLCombinedModel::Rotate(float angle, float axisX, float axisY, float axisZ){
	modelMatrix[stackPosition] *= glm::rotate(angle,vec3(axisX,axisY,axisZ));
	return *this;
}
GLCombinedModel & GLCombinedModel::Scale(vec3 scale){
	modelMatrix[stackPosition] *= glm::scale(scale);
	return *this;
}
GLCombinedModel & GLCombinedModel::Scale(float xScale, float yScale, float zScale){
	modelMatrix[stackPosition] *= glm::scale(vec3(xScale,yScale,zScale));
	return *this;
}

GLCombinedModel & GLCombinedModel::Combine(mat4 matrix) {
	modelMatrix[stackPosition] *= matrix;
	return *this;
}

GLCombinedModel & GLCombinedModel::Project(vec3 direction, vec3 pointOnPlane, vec3 planeNormal) {
	glm::mat3 am;
	am[0].x = am[1].y = am[2].z = glm::dot(direction,planeNormal);
	glm::mat3 bm;
	bm[0].x = direction.x*planeNormal.x;
	bm[0].y = direction.y*planeNormal.x;
	bm[0].z = direction.z*planeNormal.x;
	bm[1].x = direction.x*planeNormal.y;
	bm[1].y = direction.y*planeNormal.y;
	bm[1].z = direction.z*planeNormal.y;
	bm[2].x = direction.x*planeNormal.z;
	bm[2].y = direction.y*planeNormal.z;
	bm[2].z = direction.z*planeNormal.z;

	//bm = glm::transpose(bm);

	glm::mat4 proj;
	glm::mat3 inner = am-bm;
	proj[0] = vec4(inner[0],0);
	proj[1] = vec4(inner[1],0);
	proj[2] = vec4(inner[2],0);

	vec3 trans = direction*glm::dot(pointOnPlane,planeNormal);
	proj[3] = vec4(trans,glm::dot(direction,planeNormal));

	modelMatrix[stackPosition] *= proj;

	return *this;
}
