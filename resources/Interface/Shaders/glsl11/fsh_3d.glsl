
//We need this information to calculate the camera angle
varying mediump vec4 fragment_color_in;
varying mediump vec2 texture_coordinates;
varying mediump float fogFactor;

//Texture input, by default binds to "0"
uniform sampler2D basic_texture;

//The color to use for the fog
uniform mediump vec4 fogColor;

void main () 
{
	// Base fragment color - lighting stage * texture
	mediump vec4 finalColor = fragment_color_in * vec4(texture2D(basic_texture, texture_coordinates).rgb, 1.0);
    
	//Now mix in the final color
	finalColor = finalColor*(1.0-fogFactor)+fogColor*fogFactor;
	
	// Output the final fragment color
	gl_FragColor = finalColor;
}

