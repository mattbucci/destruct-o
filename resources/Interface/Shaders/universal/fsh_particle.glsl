// Input fragment color from vertex lighting stage
in highp vec4 color;

// Input texture coordinate
in highp vec2 texture_coordinates;


//The color to use for the fog
uniform highp vec4 fogColor;

//Texture input, by default binds to "0"
uniform sampler2D basic_texture;

FRAGMENT_COLOR_DEFINE

void main ()
{
	// Calculate the base color for the fragment (lighting * texture)
	highp vec4 finalColor = texture(basic_texture, texture_coordinates);
    
	finalColor *= color;
	
	//Now mix in the final color
	//Fog pending
	//finalColor = finalColor*(1.0-fogFactor)+fogColor*fogFactor;
	
	FRAGMENT_COLOR = finalColor;
}

