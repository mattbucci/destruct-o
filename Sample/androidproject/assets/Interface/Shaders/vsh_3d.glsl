# version 110

//The vertex coordinate
attribute vec4 vPos;

//Basic matrices
uniform mat4 vModel;
uniform mat4 vProj;
uniform mat4 vView;


//Output information to be used in fragment shader for lighting
varying vec3 position_eye, normal_eye;
varying vec2 texture_coordinates;



void main() {
	mat4 modelView = vView * vModel;

	//Calculate position of eye and of normal in camera-space
	position_eye = vec3(modelView * vec4(vPos.xyz, 1.0)).xyz;

	//Lookup normal for voxel
	float vertexIndex = vPos.w;
	//vertexIndex = vertexIndex-int(float(vertexIndex)/36.0)*36;
	vec3 vNormal = vec3(100,100,100);
	
	float normalIndex = floor(vPos.w/6.0);
	if (normalIndex < 1.0)
		vNormal = vec3(0,0,1);
	else if (normalIndex < 2.0)
		vNormal = vec3(0,0,-1);
	else if (normalIndex < 3.0)
		vNormal = vec3(1,0,0);
	else if (normalIndex < 4.0)
		vNormal = vec3(-1,0,0);
	else if (normalIndex < 5.0)
		vNormal = vec3(0,1,0);
	else if (normalIndex < 6.0)
		vNormal = vec3(0,-1,0);
	
	
	//Assume normals have already been normalized
	vec4 normal = modelView * vec4(vNormal.x,vNormal.y,vNormal.z,0);
	normal_eye = normal.xyz;

	texture_coordinates = vec2(0.0,0.0);//vTexture;
	gl_Position = vProj * modelView * vec4 ( vPos.xyz, 1.0 );
}
