
#ifndef GL_ES
#define highp
#define lowp
#define mediump
#endif

//The vertex coordinate
attribute highp vec4 vPos;

//Basic matrices
uniform mat4 vModel;
uniform mat4 vProj;
uniform mat4 vView;


//Output information to be used in fragment shader for lighting
varying vec3 position_eye, normal_eye;
varying vec2 texture_coordinates;
varying float fragment_depth;


void main() { 
	mat4 modelView = vView * vModel;

    //Correct and decode the coordinate
    //the negative bit is used to encode
    //which texture coordinate is used
    //when in fact all coordinates should be positive
    vec3 cPos = vPos.xyz;
    float textureCoordinateNumber = 0.0;
    if (cPos.x < 0.0) {
        cPos.x = -cPos.x;
        textureCoordinateNumber += 1.0;
    }
    if (cPos.y < 0.0) {
        cPos.y = -cPos.y;
        textureCoordinateNumber += 2.0;
    }

	//Calculate position of eye and of normal in camera-space
	position_eye = vec3(modelView * vec4(cPos.xyz, 1.0)).xyz;

	//The material id is the thousands place so
	//cut it out so we can calculate the vertexIndex alone
	highp float vertexIndex = mod(vPos.w,100.0);
	//The information lookups are designed based off of
	//"pushVoxel" in VoxelSystem.cpp
	vec3 vNormal = vec3(100,100,100);
	
	//Lookup normal for voxel
	highp float normalIndex = vertexIndex;
	if (normalIndex < 1.0)
		vNormal = vec3(0.0,0.0,1.0);
	else if (normalIndex < 2.0) 
		vNormal = vec3(0.0,0.0,-1.0);
	else if (normalIndex < 3.0)
		vNormal = vec3(1.0,0.0,0.0);
	else if (normalIndex < 4.0)
		vNormal = vec3(-1.0,0.0,0.0); 
	else if (normalIndex < 5.0)
		vNormal = vec3(0.0,1.0,0.0);
	else if (normalIndex < 6.0)
		vNormal = vec3(0.0,-1.0,0.0);

    vec2 texCoordinate;
	//Now lookup the texture coordinate
	if (textureCoordinateNumber < 1.0)
		texCoordinate = vec2(0.0,0.0);
	else if (textureCoordinateNumber < 2.0)
		texCoordinate = vec2(1.0,0.0);
	else if (textureCoordinateNumber < 3.0)
		texCoordinate = vec2(0.0,1.0);
	else if (textureCoordinateNumber < 4.0)
		texCoordinate = vec2(1.0,1.0);
	
	//Now mix in the material id
	highp float materialId = floor(vPos.w/100.0);
	texCoordinate += vec2(mod(materialId,4.0),  floor(materialId/4.0));
	//Finally scale the coordinate to fit the texture
	texture_coordinates = texCoordinate/4.0;
	
	//Assume normals have already been normalized
	vec4 normal = modelView * vec4(vNormal.x,vNormal.y,vNormal.z,0);
	normal_eye = normal.xyz;

	fragment_depth = abs(position_eye.z-cPos.z);
	gl_Position = vProj * modelView * vec4 ( cPos.xyz, 1.0 );
}
