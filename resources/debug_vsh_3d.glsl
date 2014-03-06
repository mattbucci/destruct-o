#version 140
#define VERTEX_SHADER
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
//The vertex coordinate
in highp vec3 vVert;
in highp vec2 vTex;
in highp vec3 vNorm;
in highp vec4 vPos;

//Basic matrices
//uniform mat4 vModel;
//uniform mat4 vProj;
uniform mat4 vView;

// Precalculated matricies
uniform mat4 MV;
uniform mat4 MVP;

//Count of lights to render
uniform int light_count;

//Uniforms represent properties of all possible lights
uniform highp vec3 light1_intensity;
uniform highp vec3 light1_color;
uniform highp vec4 light1_components;
uniform highp vec3 light1_position;
uniform highp vec3 light2_intensity;
uniform highp vec3 light2_color;
uniform highp vec4 light2_components;
uniform highp vec3 light2_position;
uniform highp vec3 light3_intensity;
uniform highp vec3 light3_color;
uniform highp vec4 light3_components;
uniform highp vec3 light3_position;

//Material properties follow including: color,reflectivity,texture
//Uniforms representing the current material properties

//Used to color the texture, if material_color.w is <= -1 that signifies that there is no texture
//and to just render material_color.xyz(w+2)
uniform highp vec4 material_color;

//x is the specular component and y is the specular strength or dullness
uniform highp vec2 material_reflectivity;

//Fog data
//The distance at which the fog is complete (fades in over the last 1/4th of the distance)
uniform highp float maxDistance;

// Output data to fragment stage
out highp vec4 fragment_color_in;
out highp float fogFactor;
out highp vec2 texture_coordinates;

vec4 ApplyLight(vec3 position_eye, vec3 normal_eye, vec3 intensity, vec3 color, vec4 components, vec3 position)
{
	float specular_exponent = 20.0/components.w;
	vec3 Ls = color;
	vec3 Ld = color;
	vec3 La = color;
    
	//Will be uniform-based in the future
	// surface reflectance
	vec3 Ks = vec3 (1.0, 1.0, 1.0)*(components.z*material_reflectivity.x); // fully reflect specular light
	vec3 Kd = vec3 (1.0, 1.0, 1.0)*components.y; // orange diffuse surface reflectance
	vec3 Ka = vec3 (1.0, 1.0, 1.0)*components.x; // fully reflect ambient light
    
	// ambient intensity
	vec3 Ia = La * Ka;
    
	// diffuse intensity
	// raise light position to eye space
	vec3 light_position_eye = vec3(vView * vec4(position, 1.0));
	vec3 distance_to_light_eye = light_position_eye - position_eye;
	vec3 direction_to_light_eye = normalize (distance_to_light_eye);
	float dot_prod = dot (direction_to_light_eye, normal_eye);
	dot_prod = max (dot_prod, 0.0);
	vec3 Id = Ld * Kd * dot_prod; // final diffuse intensity
	
	//Calculate light distance here because calculating specular values is very intensive so
	//if we can skip it for any reason, we will
	//Apply distance of light to reduce lighting effect
	float dist = sqrt(dot(distance_to_light_eye,distance_to_light_eye));
	float lightStrength = max(0.0,1.0 - intensity.x - intensity.y*dist - intensity.z*dist*dist);
    
	
	vec3 Is = vec3(0,0,0);
	// specular intensity
	if ((Ks.z > 0.0) && (lightStrength > 0.0)) {
		vec3 reflection_eye = reflect (-direction_to_light_eye, normal_eye);
		vec3 surface_to_viewer_eye = normalize (-position_eye);
		float dot_prod_specular = dot (reflection_eye, surface_to_viewer_eye);
		dot_prod_specular = max (dot_prod_specular, 0.0);
		float specular_factor = pow (dot_prod_specular, specular_exponent * max(.1,material_reflectivity.y));
		Is = Ls * Ks * specular_factor; // final specular intensity
	}
	
	//Combine into lighted color
	vec4 lightedPixel = vec4(Is + Id + Ia, 1.0);
    
	return lightedPixel * lightStrength;
}

void main()
{
	//mat4 modelView = vView * vModel;
    
	//Calculate position of eye and of normal in camera-space
	vec3 vertexPosition = vVert + vPos.xyz;
	//vec3 position_eye = vec3(modelView * vec4(vertexPosition, 1.0)).xyz;
	vec3 position_eye = (MV * vec4(vertexPosition, 1.0)).xyz;
	
	//Assume normals have already been normalized
	vec3 normal_eye = (MV * vec4(vNorm,0)).xyz;

	//Now mix in the material id
	vec2 texCoordinate = vTex;
	float materialId = vPos.w;
	texCoordinate += vec2(mod(materialId,4.0),  floor(materialId/4.0));
    
	//Finally scale the coordinate to fit the texture
	texture_coordinates = texCoordinate/4.0;
    
    //Fragment starts black and then we add the lighting
	vec4 fcolor = vec4(0,0,0,0);
    
	//Calculate lighting and surface properties using all available lights
	if (light_count >= 1)
		fcolor += ApplyLight(position_eye, normal_eye, light1_intensity,light1_color,light1_components,light1_position);
	if (light_count >= 2)
		fcolor += ApplyLight(position_eye, normal_eye, light2_intensity,light2_color,light2_components,light2_position);
	if (light_count >= 3)
		fcolor += ApplyLight(position_eye, normal_eye, light3_intensity,light3_color,light3_components,light3_position);
    
	//No lights enabled means render no lighting (everything equally lit)
	if (light_count == 0)
		fcolor = vec4(1,1,1,1);
    
    // Make sure its opaque
    fragment_color_in = vec4(fcolor.rgb, 1.0);
    
    //Handle fog now
	highp float partDistance = (maxDistance/4.0);
    
	//Calculate how much fog should effect the current pixel
	fogFactor = max(min((abs(position_eye.z) - partDistance*3.0)/partDistance,1.0),0.0);
    
    // Calculate the position of the vertex
	//gl_Position = vProj * modelView * vec4 ( vertexPosition, 1.0 );
	gl_Position = acidTransform(MVP * vec4 ( vertexPosition, 1.0 ));
}
 