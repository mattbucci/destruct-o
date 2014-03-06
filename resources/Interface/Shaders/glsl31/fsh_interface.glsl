
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
  