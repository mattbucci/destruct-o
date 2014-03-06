
in vec2 vPos;
in vec2 vTex;

//x,y are texture coordinates
//z,w are fragment coordinates
out mediump vec4 fragmentData;

uniform float uWidth;
uniform float uHeight;
uniform mat4 uModel;

void main () {

	vec4 beforeTransform = vec4(vPos.x,vPos.y,0.0,1.0);
	beforeTransform = uModel * beforeTransform;

	//Modify vertex coordinates so that they go from 0 to uWidth/0 to uHeight instead of
	//-1 to 1
	//also 0,0 is top left
	vec4 pos = vec4(beforeTransform.x/uWidth*2.0-1.0,1.0-beforeTransform.y/uHeight*2.0,0.0, 1.0);
	//Also pass the position to the fragment shader so it can perform "clipping"
	fragmentData = vec4(vTex.x,vTex.y,beforeTransform.x,beforeTransform.y);
	gl_Position = pos;
}
