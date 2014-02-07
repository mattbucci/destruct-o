#version 140

//Position and fragment coordiantes are packaged into fragmentData
in vec4 fragmentData;

//1 for yes, 0 for no
uniform float textureEnabled;

//Clipping region defined as X1,Y1,X2,Y2 
//X1 = -1 if no clipping region enabled
uniform vec4 clippingRegion;
 
//Color of material
uniform vec4 uColor; 

//Texture input, by default binds to "0"
uniform sampler2D basic_texture;

out vec4 fragmentColor;

void main() { 
	vec2 pos = fragmentData.xy;
	vec2 sFragmentCords = fragmentData.zw;  
	//First check if the fragment is within the allowed window region
	if ((clippingRegion.x >= 0.0) && ((sFragmentCords.x < clippingRegion.x) || (sFragmentCords.x > clippingRegion.z)
			|| (sFragmentCords.y < clippingRegion.y) || (sFragmentCords.y > clippingRegion.w))) {
		//Discard the fragment
		discard;
	}
	//If textures are enabled retrieve a texel
	else if (textureEnabled >= 1.0) { 
		vec4 texel = texture(basic_texture, pos,0.0);
		fragmentColor = texel * uColor;
	}
	//Otherwise paint straight color
	else
		fragmentColor = uColor;
	
} 
  