
//The vertex coordinate
attribute mediump vec4 vPos;

//Basic matrices
//uniform mediump mat4 vModel;
//uniform mediump mat4 vProj;
uniform mediump mat4 vView;

// Precalculated matricies
uniform mediump mat4 MV;
uniform mediump mat4 MVP;

//Count of lights to render
uniform int light_count;

//Uniforms represent properties of all possible lights
uniform mediump vec3 light1_intensity;
uniform mediump vec3 light1_color;
uniform mediump vec4 light1_components;
uniform mediump vec3 light1_position;
uniform mediump vec3 light2_intensity;
uniform mediump vec3 light2_color;
uniform mediump vec4 light2_components;
uniform mediump vec3 light2_position;
uniform mediump vec3 light3_intensity;
uniform mediump vec3 light3_color;
uniform mediump vec4 light3_components;
uniform mediump vec3 light3_position;

//Material properties follow including: color,reflectivity,texture
//Uniforms representing the current material properties

//Used to color the texture, if material_color.w is <= -1 that signifies that there is no texture
//and to just render material_color.xyz(w+2)
uniform mediump vec4 material_color;
//x is the specular component and y is the specular strength or dullness
uniform mediump vec2 material_reflectivity;

//Fog data
//The distance at which the fog is complete (fades in over the last 1/4th of the distance)
uniform mediump float maxDistance;


//Output information to be used in fragment shader for lighting
//varying mediump vec3 position_eye, normal_eye;
varying mediump vec4 fragment_color_in;
varying mediump vec2 texture_coordinates;
varying mediump float fogFactor;

mediump vec4 ApplyLight(mediump vec3 position_eye, mediump vec3 normal_eye, mediump vec3 intensity, mediump vec3 color, mediump vec4 components, mediump vec3 position)
{
	mediump float specular_exponent = 20.0/components.w;
	mediump vec3 Ls = color;
	mediump vec3 Ld = color;
	mediump vec3 La = color;
    
	//Will be uniform-based in the future
	// surface reflectance
	mediump vec3 Ks = vec3 (1.0, 1.0, 1.0)*(components.z*material_reflectivity.x); // fully reflect specular light
	mediump vec3 Kd = vec3 (1.0, 1.0, 1.0)*components.y; // orange diffuse surface reflectance
	mediump vec3 Ka = vec3 (1.0, 1.0, 1.0)*components.x; // fully reflect ambient light
    
	// ambient intensity
	mediump vec3 Ia = La * Ka;
    
	// diffuse intensity
	// raise light position to eye space
	mediump vec3 light_position_eye = vec3(vView * vec4(position, 1.0));
	mediump vec3 distance_to_light_eye = light_position_eye - position_eye;
	mediump vec3 direction_to_light_eye = normalize (distance_to_light_eye);
	mediump float dot_prod = dot (direction_to_light_eye, normal_eye);
	dot_prod = max (dot_prod, 0.0);
	mediump vec3 Id = Ld * Kd * dot_prod; // final diffuse intensity
	
	//Calculate light distance here because calculating specular values is very intensive so
	//if we can skip it for any reason, we will
	//Apply distance of light to reduce lighting effect
	mediump float dist = sqrt(dot(distance_to_light_eye,distance_to_light_eye));
	mediump float lightStrength = max(0.0,1.0 - intensity.x - intensity.y*dist - intensity.z*dist*dist);
    
	
	mediump vec3 Is = vec3(0,0,0);
	// specular intensity
	if ((Ks.z > 0.0) && (lightStrength > 0.0)) {
		mediump vec3 reflection_eye = reflect (-direction_to_light_eye, normal_eye);
		mediump vec3 surface_to_viewer_eye = normalize (-position_eye);
		mediump float dot_prod_specular = dot (reflection_eye, surface_to_viewer_eye);
		dot_prod_specular = max (dot_prod_specular, 0.0);
		mediump float specular_factor = pow (dot_prod_specular, specular_exponent * max(.1,material_reflectivity.y));
		Is = Ls * Ks * specular_factor; // final specular intensity
	}
	
	//Combine into lighted color
	mediump vec4 lightedPixel = vec4(Is + Id + Ia, 1.0);
    
	return lightedPixel * lightStrength;
}

void main() { 
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
	vec3 position_eye = vec3(MV * vec4(cPos.xyz, 1.0)).xyz;

	//The material id is the thousands place so
	//cut it out so we can calculate the vertexIndex alone
	mediump float vertexIndex = mod(vPos.w,100.0);
	//The information lookups are designed based off of
	//"pushVoxel" in VoxelSystem.cpp
	vec3 vNormal = vec3(100,100,100);
	
	//Lookup normal for voxel
	mediump float normalIndex = vertexIndex;
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
	mediump float materialId = floor(vPos.w/100.0);
	texCoordinate += vec2(mod(materialId,4.0),  floor(materialId/4.0));
	//Finally scale the coordinate to fit the texture
	texture_coordinates = texCoordinate/4.0;
	
	//Assume normals have already been normalized
	vec4 normal = MV * vec4(vNormal.x,vNormal.y,vNormal.z,0);
    
    //Fragment starts black and then we add the lighting
	mediump vec4 fcolor = vec4(0,0,0,0);
    
    //Calculate lighting and surface properties using all available lights
	if (light_count >= 1)
		fcolor += ApplyLight(position_eye, normal.xyz, light1_intensity,light1_color,light1_components,light1_position);
	if (light_count >= 2)
		fcolor += ApplyLight(position_eye, normal.xyz, light2_intensity,light2_color,light2_components,light2_position);
	if (light_count >= 3)
		fcolor += ApplyLight(position_eye, normal.xyz, light3_intensity,light3_color,light3_components,light3_position);
    
	//No lights enabled means render no lighting (everything equally lit
	if (light_count == 0)
		fcolor = vec4(1,1,1,1);
    
    // Fragment color out
    fragment_color_in = vec4(fcolor.xyz, 1.0);
	
	//Handle fog now
	mediump float partDistance = (maxDistance/4.0);
    
	//Calculate how much fog should effect the current pixel
	fogFactor = max(min((abs(position_eye.z) - partDistance*3.0)/partDistance,1.0),0.0);
	
    // Set the vertex position
	gl_Position = acidTransform(MVP * vec4 ( cPos.xyz, 1.0 ));
}
