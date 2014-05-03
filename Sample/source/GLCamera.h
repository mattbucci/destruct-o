
#pragma once
#include "stdafx.h"

class GLCombinedMatrix;

class GLCamera
{
    // Camera related matrices
	mat4 viewMatrix;
	mat4 projectionMatrix;
    
    // Up vector
	vec3 cameraZAxis;
	
    // OpenGL uniforms for matrices
    GLint uniformMatrixProjection;
    GLint uniformMatrixView;
    GLint uniformMatrixCombined;
    
public:
	GLCamera(GLint uniformMatrixProjection, GLint uniformMatrixView, GLint uniformMatrixCombined);
	
	//Copy the view or projection matrices the camera is using
	//point may be NULL to not copy that value
	void CopyMatricies(mat4 * viewMatrix, mat4 * projectionMatrix) const;

	void SetViewMatrix(mat4 viewMatrix);

	//Retrieve the facing axis of the camera
	vec3 GetZAxis();

	//Apply the camera projection/view matrix
	void Apply();

	//Reset the view and projection matrix to the identity matrix
	void Reset();

	//Set the camera position in the world. Up vector defaults to 0,0,1
	void SetCameraPosition(vec3 cameraPos, vec3 target, vec3 upVector = vec3(0,0,1));

	//Change the frustrum matrix to the given values
	void SetFrustrum(float verticleFOV, float aspectRatio, float nearDistance, float farDistance);
    
    // Get the view matrix
    const mat4& ViewMatrix() const;
    
    // Get the projection matrix
    const mat4& ProjectionMatrix() const;
};