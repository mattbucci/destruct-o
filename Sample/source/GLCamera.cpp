#include "stdafx.h"
#include "GLCamera.h"
#include "GLCombinedMatrix.h"

GLCamera::GLCamera(GLint uniformMatrixProjection, GLint uniformMatrixView, GLint uniformMatrixCombined)
{
	this->uniformMatrixProjection = uniformMatrixProjection;
    this->uniformMatrixView = uniformMatrixView;
    this->uniformMatrixCombined = uniformMatrixCombined;
}
	

void GLCamera::CopyMatricies(mat4 * viewMatrix, mat4 * projectionMatrix) const
{
	if (viewMatrix != NULL)
		*viewMatrix = this->viewMatrix;
	if (projectionMatrix != NULL)
		*projectionMatrix = this->projectionMatrix;
}

void GLCamera::SetViewMatrix(mat4 viewMatrix) {
	this->viewMatrix = viewMatrix;	
}

//Apply the camera projection/view matrix
void GLCamera::Apply()
{
    // Apply the project matrix
    if(uniformMatrixProjection >= 0)
    {
        glUniformMatrix4fv(uniformMatrixProjection, 1, GL_FALSE, &projectionMatrix[0][0]);
    }
    
    // Apply the view matrix
    if(uniformMatrixView >= 0)
    {
        glUniformMatrix4fv(uniformMatrixView, 1, GL_FALSE, &viewMatrix[0][0]);
    }
    
    // Apply the precomputed matrix if required
    if(uniformMatrixCombined >= 0)
    {
        mat4 combined = projectionMatrix * viewMatrix;
        glUniformMatrix4fv(uniformMatrixCombined, 1, GL_FALSE, &combined[0][0]);
    }
}

//Reset the view and projection matrix to the identity matrix
void GLCamera::Reset() {
	viewMatrix = mat4();
	projectionMatrix = mat4();
}

//Set the camera position in the world. Up vector defaults to 0,0,1
void GLCamera::SetCameraPosition(vec3 cameraPos, vec3 target, vec3 upVector) {

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