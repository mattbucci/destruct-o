#include "stdafx.h"
#include "GLCamera.h"

GLCamera::GLCamera(GLint uniformViewMatrix, GLint uniformProjectionMatrix) {
	_ASSERTE(uniformProjectionMatrix > 0);
	_ASSERTE(uniformViewMatrix > 0);

	this->uniformViewMatrix = uniformViewMatrix;
	this->uniformProjectionMatrix = uniformProjectionMatrix;
}
	

void GLCamera::CopyMatricies(mat4 * viewMatrix, mat4 * projectionMatrix) {
	if (viewMatrix != NULL)
		*viewMatrix = this->viewMatrix;
	if (projectionMatrix != NULL)
		*projectionMatrix = this->projectionMatrix;
}

void GLCamera::SetViewMatrix(mat4 viewMatrix) {
	this->viewMatrix = viewMatrix;	
}

//Apply the camera projection/view matrix
void GLCamera::Apply() {
	//Apply the matrices to the shaders
	glUniformMatrix4fv(uniformViewMatrix,1,GL_FALSE,(const GLfloat*)&viewMatrix);
	glUniformMatrix4fv(uniformProjectionMatrix,1,GL_FALSE,(const GLfloat*)&projectionMatrix);
}

//Reset the view and projection matrix to the identity matrix
void GLCamera::Reset() {
	viewMatrix = mat4();
	projectionMatrix = mat4();
}

//Set the camera position in the world. Up vector defaults to 0,0,1
void GLCamera::SetCameraPosition(vec3 cameraPos, vec3 target, vec3 upVector) {
	/*viewMatrix = mat4();
	viewMatrix[3].x = -cameraPos.z;
	viewMatrix[3].y = -cameraPos.y;
	viewMatrix[3].z = -cameraPos.x;

	vec3 forwardVector = cameraZAxis = glm::normalize(target-cameraPos);
	vec3 rightVector = glm::normalize(glm::cross(forwardVector,upVector));
	upVector = glm::cross(rightVector,forwardVector);
	
	viewMatrix[0].x = rightVector.x;
	viewMatrix[1].x = rightVector.y;
	viewMatrix[2].x = rightVector.z;
	viewMatrix[0].y = upVector.x;
	viewMatrix[1].y = upVector.y;
	viewMatrix[2].y = upVector.z;
	viewMatrix[0].z = -forwardVector.x;
	viewMatrix[1].z = -forwardVector.y;
	viewMatrix[2].z = -forwardVector.z;*/

	vec3 zaxis = cameraZAxis = glm::normalize(cameraPos - target);
	vec3 xaxis = glm::normalize(glm::cross(upVector,zaxis));
	vec3 yaxis = glm::cross(zaxis,xaxis);
	viewMatrix[0] = vec4(xaxis,-glm::dot(xaxis,cameraPos));
	viewMatrix[1] = vec4(yaxis,-glm::dot(yaxis,cameraPos));
	viewMatrix[2] = vec4(zaxis,-glm::dot(zaxis,cameraPos));
	viewMatrix[3] = vec4(0,0,0,1);
	viewMatrix = glm::transpose(viewMatrix);

}

vec3 GLCamera::GetZAxis() {
	return cameraZAxis;
}

/*
   float top = znear * tanf ( fovy/2.0f );
   float left = -top * aspect;

   float w = -left-left; // width
   float h = top+top;    // height
   float z = zfar-znear; // znear and zfar are > 0

   setl1 (    (2*znear)/w,           0,                 0,  0 );
   setl2 (              0, (2*znear)/h,                 0,  0 );
   setl3 (              0,           0,   -(zfar+znear)/z, -1 );
   setl4 (              0,           0, -(2*zfar*znear)/z,  0 );
*/

//Change the frustrum matrix to the given values
void GLCamera::SetFrustrum(float verticalFOV, float aspectRatio, float nearDistance, float farDistance) {
	float range = (float)tan(verticalFOV*3.141592/180.0*.5)*nearDistance;
	memset(&projectionMatrix,0,sizeof(projectionMatrix));
	projectionMatrix[0].x = (2.0f*nearDistance)/(2.0f*range*aspectRatio);
	
	projectionMatrix[1].y = nearDistance/range;
	projectionMatrix[2].z = -(farDistance+nearDistance)/(farDistance-nearDistance);
	projectionMatrix[3].z = -(2.0f*farDistance*nearDistance)/(farDistance-nearDistance);
	projectionMatrix[2].w = -1;
}