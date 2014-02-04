# version 110

varying vec2 TexCoordinates;
varying vec3 cords;

//1 for yes, 0 for no
uniform int textureEnabled;

//Clipping region defined as X1,Y1,X2,Y2
//X1 = -1 if no clipping region enabled
uniform vec4 clippingRegion;

//Color of material
uniform vec4 uColor;

//Texture input, by default binds to "0"
uniform sampler2D basic_texture;

void main() { 
	if (clippingRegion.x >= 0.0) {
		if ((cords.x < clippingRegion.x) || (cords.x > clippingRegion.z)
			|| (cords.y < clippingRegion.y) || (cords.y > clippingRegion.w)) {

			gl_FragColor = vec4(1.0,1.0,1.0,.0); //If outside the clipping region paint transparency
			return;
		}
	}

	if (textureEnabled == 1) {
		vec4 texel = texture2D(basic_texture, TexCoordinates);
		gl_FragColor = texel * uColor;
		//fColor = vec4(0.0,0.0,1.0,1.0);
	}
	else {
		gl_FragColor = uColor;
	}
} 
