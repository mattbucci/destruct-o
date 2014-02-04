# version 110

//We need this information to calculate the camera angle
varying vec3 position_eye, normal_eye;
varying vec2 texture_coordinates;
//Count of lights to render
uniform int light_count;
//Uniforms represent properties of all possible lights
uniform vec3 light1_intensity;
uniform vec3 light1_color;
uniform vec4 light1_components;
uniform vec3 light1_position;
uniform vec3 light2_intensity;
uniform vec3 light2_color;
uniform vec4 light2_components;
uniform vec3 light2_position;
uniform vec3 light3_intensity;
uniform vec3 light3_color;
uniform vec4 light3_components;
uniform vec3 light3_position;
//We need the view_matrix as well, to calculate lighting

uniform mat4 vView;

//Material properties follow including: color,reflectivity,texture
//Uniforms representing the current material properties

//Used to color the texture, if material_color.w is <= -1 that signifies that there is no texture
//and to just render material_color.xyz(w+2)
uniform vec4 material_color;
//x is the specular component and y is the specular strength or dullness
uniform vec2 material_reflectivity;
//Texture input, by default binds to "0"
uniform sampler2D basic_texture;


vec4 ApplyLight(vec3 intensity, vec3 color, vec4 components, vec3 position) {
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

void main () {
	//Fragment starts black and then we add the lighting
	vec4 fcolor = vec4(0,0,0,0);

	//Calculate lighting and surface properties using all available lights
	if (light_count >= 1)
		fcolor += ApplyLight(light1_intensity,light1_color,light1_components,light1_position);
	if (light_count >= 2)
		fcolor += ApplyLight(light2_intensity,light2_color,light2_components,light2_position);
	if (light_count >= 3)
		fcolor += ApplyLight(light3_intensity,light3_color,light3_components,light3_position);

	//No lights enabled means render no lighting (everything equally lit
	if (light_count == 0)
		fcolor = vec4(1,1,1,1);

	//vec4 texel = texture2D(basic_texture, texture_coordinates);
	
	gl_FragColor = fcolor*vec4(1,1,0,1);//*texel;
}

