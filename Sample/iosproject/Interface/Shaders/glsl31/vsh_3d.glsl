#version 140

//The vertex coordinate
in vec3 vVert;
in vec2 vTex;
in vec3 vNorm;
in vec4 vPos;

//Basic matrices
uniform mat4 vModel;
uniform mat4 vProj;
uniform mat4 vView;


//Output information to be used in fragment shader for lighting
out vec3 position_eye, normal_eye;
out vec2 texture_coordinates;
out float fragment_depth;

void main() { 
	mat4 modelView = vView * vModel;

	//Calculate position of eye and of normal in camera-space
	vec3 vertexPosition = vVert + vPos.xyz;
	position_eye = vec3(modelView * vec4(vertexPosition, 1.0)).xyz;

	
	//Now mix in the material id
	vec2 texCoordinate = vTex;
	float materialId = vPos.w;
	texCoordinate += vec2(mod(materialId,4.0),  floor(materialId/4.0));
	//Finally scale the coordinate to fit the texture
	texture_coordinates = texCoordinate/4.0;
	
	//Assume normals have already been normalized
	vec4 normal = modelView * vec4(vNorm,0);
	normal_eye = normal.xyz;

	fragment_depth = abs(position_eye.z-vertexPosition.z);
	gl_Position = vProj * modelView * vec4 ( vertexPosition, 1.0 );
}
