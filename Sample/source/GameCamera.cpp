#include "stdafx.h"
#include "GameCamera.h"
#include "OS.h"

#ifndef M_PI
#define M_PI 3.141592f
#endif

GameCamera::GameCamera() {
	
}

void GameCamera::UpdateViewSize(vec2 viewSize) {
	viewPortSize = viewSize;
}

void GameCamera::Draw(GL3DProgram * shaders) {
	shaders->Camera.SetCameraPosition(position,position+direction,vec3(0,0,1));
	//IF YOU CHANGE NEAR/FAR CLIPPING PLANE, PLEASE CHANGE UNPROJECT (below) AS WELL
	shaders->Camera.SetFrustrum(60,viewPortSize.x/viewPortSize.y,1,1000); //width/height
	shaders->Camera.Apply();


	//Setup sun here for now
	//translate it to follow world coordinates
	shaders->Lights.Enable(1);
	shaders->Lights[0].SetColor(vec3(1,1,1));
	shaders->Lights[0].SetIntensity(0,0,0); //The sun shines infinitely long
	shaders->Lights[0].SetComponents(.4f,1,1,20);
	shaders->Lights[0].SetLocation(shaders->Model.TransformVector(vec3(-200000,-200000,400000)));
	shaders->Lights.Apply();

	//Copy matrices for unproject
	shaders->Camera.CopyMatricies(&lastViewMatrix,&lastProjectionMatrix);
	lastModelMatrix = shaders->Model.GetMatrix();
}

//Move the camera view point
void GameCamera::SetCameraView(vec3 position, vec3 direction) {
	this->position = position;
	this->direction = direction;
}
//get camera position
vec3 GameCamera::GetPosition() {
	return position;
}
//get camera direction
vec3 GameCamera::GetDirection() {
	return direction;
}

//Cut's dependency on gluUnproject
//made frome code found at
//http://www.opengl.org/wiki/GluProject_and_gluUnProject_code
static void unproject(vec3 win,mat4 modelView, mat4 projection, GLint * viewport, vec3 * projected) {
	//Calculation for inverting a matrix, compute projection x modelview
	 //and store in A[16]
	mat4 A = projection * modelView;
	//Now compute the inverse of matrix A
	mat4 m = glm::inverse(A);
	//Transformation of normalized coordinates between -1 and 1
	vec4 in = vec4((win.x-(float)viewport[0])/(float)viewport[2]*2.0f-1.0f,
				  (win.y-(float)viewport[1])/(float)viewport[3]*2.0f-1.0f,
					2.0f*win.z-1.0f,
					1.0f);
	//Objects coordinates
	vec4 out = m * in;
	//fail quietly
	if (out.w == 0.0f)
		return;
	out.w = 1.0f/out.w;
	projected->x = out.x*out.w;
	projected->y = out.y*out.w;
	projected->z = out.z*out.w;
}

pair<vec3,vec3> GameCamera::Unproject(vec2 pos) {
	//First unproject two points, near and far clipping plane
	vec3 vnear = vec3(pos.x,pos.y,10);
	vec3 vfar = vec3(pos.x,pos.y,2000);
	GLint view[4];
	mat4 modelview = lastViewMatrix * lastModelMatrix;
	
	view[0] = 0;
	view[1] = 0;
	view[2] = (int)viewPortSize.x;
	view[3] = (int)viewPortSize.y;

	vec3 unear;
	vec3 ufar;
	
	unproject(vnear,modelview,lastProjectionMatrix,view,&unear);
	unproject(vfar,modelview,lastProjectionMatrix,view,&ufar);

	//Can't normalize in this case
	if (vfar == vnear)
		return pair<vec3,vec3>(unear,vec3());

	//This is actually backwards:
	//but now the rest of the code depends on the backwards
	//(not a big deal though)
	return pair<vec3,vec3>(unear,glm::normalize(unear-ufar));
}

//Calculates when a ray emanating from rayStart going towards rayDirection
//will come in contact with the plane defined by planePoint, planeNormal
double CalculateIntersectionDistance(vec3 rayStart, vec3 rayDirection, vec3 planePoint, vec3 planeNormal) {
	/*double D = glm::dot(planePoint,planeNormal);
	double s = D-(glm::dot(rayStart,planeNormal));
	return s/(glm::dot(rayDirection,planeNormal));*/
	double topPart = glm::dot((planePoint-rayStart),planeNormal);
	double bottomPart = glm::dot(rayDirection,planeNormal);
	//Cut off extremely long distances

	return topPart/bottomPart;
}


vec2 GameCamera::UnprojectToGround(vec2 pos, float groundHeight) {
	pair<vec3,vec3> unprojected = Unproject(pos);
	//Now lets do some simple math to figure out where a vector with position,normal
	//intersects with plane
	double dist = CalculateIntersectionDistance(unprojected.first,unprojected.second,vec3(0,0,groundHeight),vec3(0,0,1));

	//If the dist isn't right, (it's behind you) correct it as much as possible
	if (dist > 0) {
		double x = 0;
		double y = 0;
		const static double value = 100000;
		if (unprojected.second.x < 0)
			x = value;
		else
			x = -value;
		if (unprojected.second.y < 0) 
			y = value;
		else
			y = -value;
		return vec2((float)x,(float)y);
	}

	//Get the intersection location
	vec3 loc = unprojected.first + unprojected.second*(float)dist;
	//z has to be zero since the plane has point 0,0,0
	return vec2(loc.x,loc.y);
}
