
#pragma once

#include "stdafx.h"

#include "GL3DProgram.h"
#include "SimplePolygon.h"

class GameCamera {
	vec3 position;
	vec3 direction;
    float distance;
    
	//Matrices needed for unproject
	//taken from the last frame
	mat4 lastProjectionMatrix;
	mat4 lastViewMatrix;
	mat4 lastModelMatrix;
	vec2 viewPortSize;
    float verticalFov;
    float horizontalFov;
public:
	GameCamera(float vFov = 60.0f);

	//Should be called before any unproject calls
	void UpdateViewSize(vec2 viewSize);

	//Apply the camera and global lighting
	template<class T>
	void Apply(T * shaders)
    {
		shaders->Camera.SetCameraPosition(position,position+direction,vec3(0,0,1));
		//IF YOU CHANGE NEAR/FAR CLIPPING PLANE, PLEASE CHANGE UNPROJECT (below) AS WELL
		shaders->Camera.SetFrustrum(verticalFov,viewPortSize.x/viewPortSize.y,.25,distance); //width/height
		shaders->Camera.Apply();

		//Copy matrices for unproject
		shaders->Camera.CopyMatricies(&lastViewMatrix,&lastProjectionMatrix);
		lastModelMatrix = shaders->Model.GetMatrix();
	}

	//Move the camera view point
	void SetCameraView(vec3 position, vec3 direction, float distance);
    
	//get camera position
	vec3 GetPosition();
    
	//get camera direction
	vec3 GetDirection();
    
    // get view camera distance
    float GetDistance();

    // Get the viewing area of the camera as a top down triangle
    void GetViewingArea(SimplePolygon<4>& viewingArea);
    
	//Given a screen coordinate, what 3d vector does that coordinate represent
	//in game
	//The return value is location,direction
	pair<vec3,vec3> Unproject(vec2 pos);
	//Given a screen coordinate find the location on the terrain that matches that click
	//That is, where on the ground did the user click
	//doesn't take into account the varying nature of the terrain yet
	vec2 UnprojectToGround(vec2 pos, float groundHeight = 0.0);

};