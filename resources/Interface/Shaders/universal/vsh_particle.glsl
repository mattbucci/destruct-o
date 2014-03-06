
//The particle position information
//xyz is the particle position
//w is the size of the particle
//xy use the sign bit to encode the vertex number
//++ 0
//+- 1
//-+ 2
//-- 3

in highp vec4 vVert;
//The vertex texture coordinate
in highp vec2 vTex;
//The vertex color
in highp vec4 vColor;

//Basic matrices
uniform highp mat4 uView;
uniform highp mat4 uProjection;


//Output information to be used in fragment shader
out highp vec2 texture_coordinates;
out highp vec4 color;


void main() {
	vec4 pPos = vVert;
	float vertNumber = 0.0;
	//Decode the vertex number
	if (pPos.x < 0.0) {
		vertNumber += 2.0;
		pPos.x = -pPos.x;
	}
	if (pPos.y < 0.0) {
		vertNumber += 1.0;
		pPos.y = -pPos.y;
	}
	//Now use the vertex number plus the particle scale
	//to create a vertex for this particle
	vec4 pVert = vec4(0,0,0,1.0);
	if (vertNumber < .9)
		pVert = vec4(-vVert.w,-vVert.w,0,1.0);
	else if (vertNumber < 1.9)
		pVert = vec4(vVert.w,-vVert.w,0,1.0);
	else if (vertNumber < 2.9)
		pVert = vec4(-vVert.w,vVert.w,0,1.0);		
	else
		pVert = vec4(vVert.w,vVert.w,0,1.0);	
	
	//Now generate the billboarded transformation matrix
	//for this particle
	mat4 transform = mat4(1.0);
	transform[3] = vec4(pPos.xyz,1.0);
	mat4 modelView = uView*transform;
	//Billboard the modelView
	vec4 savedTranslation = modelView[3];
	modelView = mat4(1.0);
	modelView[3] = savedTranslation;
	
	//Finally combine the billboarded modelview with the projection matrix
	mat4 MVP = uProjection * modelView;
	
	color = vColor;
	texture_coordinates = vTex;
	gl_Position = MVP * pVert;
}
