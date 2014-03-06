#version 140
#define FRAGMENT_SHADER
#define GLSL_APPLE
#define GLSL_VERSION_140

//Mobile compatibility layer
#ifdef GLSL_MOBILE
	#ifndef GLSL_IPHONEOS
	#define highp mediump
	#endif
#else
	#define highp
	#define mediump
	#define lowp
#endif
//Universal shaders across GLSL versions
#ifdef GLSL_VERSION_140
	//Setup for GLSL version 140
	//handle different fragment color output mechanisms
	#define FRAGMENT_COLOR fragmentColor
	#define FRAGMENT_COLOR_DEFINE out vec4 fragmentColor;
	
#else
	//Setup for GLSL version 110
	
	#ifndef VERTEX_SHADER
	#define in varying
	#else
	#define in attribute
	#endif
	#define out varying
		
	#define texture texture2D
		
	//handle different fragment color output mechanisms
	#define FRAGMENT_COLOR gl_FragColor
	#define FRAGMENT_COLOR_DEFINE
	
#endif

//Acid transformation function
//and time uniform
//not used in fragment shaders
//The current time in seconds from 0 to 100 (then wraps)
uniform highp float uCurrentTime;
//The amount the acid transform should effect the vertex, from 0 to 1
uniform highp float uAcidFactor;
//Transforms a vertex into the wibbly wobbly world of someone on acid (apparently)
highp vec4 acidTransform(highp vec4 originalVertex) {
	highp vec4 newVertex;
	newVertex = originalVertex;
	
	highp float distFactor = max(min(originalVertex.z/8.0,2.5),-2.5);
	//Here we add fast rolling waves which get stronger in the distance
	newVertex.y += uAcidFactor * distFactor * sin((uCurrentTime/5.0*3.141592*2.0)+originalVertex.z/10.0);
	
	//Here we add a twisting motion around a slowly rotating axis
	highp float axisAngle = uAcidFactor * sin(uCurrentTime/20.0*3.141592*2.0 + sin(originalVertex.x/30.0));

	
	//Decrease the angle the closer you are
	axisAngle *= distFactor/5.0;
	
	//The rotation point itself wanders around
	highp float rotationPointTimeFactor = uCurrentTime/50.0*3.141592*2.0;
	highp float rotationPointEffectFactor = cos(uCurrentTime/25.0*3.141592*2.0)+.3;
	highp vec2 rotationPoint = vec2(cos(rotationPointTimeFactor),sin(rotationPointTimeFactor))*rotationPointEffectFactor;
	rotationPoint *= 0.2;
	
	
	highp vec2 oV = vec2(newVertex.x-rotationPoint.x,newVertex.y-rotationPoint.y);
	newVertex.x = oV.x*cos(axisAngle)-oV.y*sin(axisAngle) + rotationPoint.x;
	newVertex.y = oV.x*sin(axisAngle)+oV.y*cos(axisAngle) + rotationPoint.y;
	
	
	
	
	return newVertex;
}

in mediump vec4 fragmentData;

//1 for yes, 0 for no
uniform mediump float textureEnabled;

//Clipping region defined as X1,Y1,X2,Y2 
//X1 = -1 if no clipping region enabled
uniform mediump vec4 clippingRegion;
 
//Color of material
uniform mediump vec4 uColor; 

//Texture input, by default binds to "0"
uniform sampler2D basic_texture;

FRAGMENT_COLOR_DEFINE

void main() { 
	mediump vec2 pos = fragmentData.xy;
	mediump vec2 sFragmentCords = fragmentData.zw;  
	if ((clippingRegion.x >= 0.0) && ((sFragmentCords.x < clippingRegion.x) || (sFragmentCords.x > clippingRegion.z)
			|| (sFragmentCords.y < clippingRegion.y) || (sFragmentCords.y > clippingRegion.w))) {
			 
		FRAGMENT_COLOR = vec4(1.0,0.0,0.0,0.0); //If outside the clipping region paint transparency
	}
	else if (textureEnabled >= 1.0) { 
		mediump vec4 texel = texture(basic_texture, pos,0.0);
		FRAGMENT_COLOR = texel * uColor;
	}
	else { 
		FRAGMENT_COLOR = uColor;
	}
} 
   