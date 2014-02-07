
varying highp vec4 fragmentData;

//1 for yes, 0 for no
uniform highp float textureEnabled;

//Clipping region defined as X1,Y1,X2,Y2 
//X1 = -1 if no clipping region enabled
uniform vec4 clippingRegion;
 
//Color of material
uniform vec4 uColor;

//Texture input, by default binds to "0"
uniform sampler2D basic_texture;


void main() { 
	mediump vec2 pos = fragmentData.xy;
	mediump vec2 sFragmentCords = fragmentData.zw;  
	if ((clippingRegion.x >= 0.0) && ((sFragmentCords.x < clippingRegion.x) || (sFragmentCords.x > clippingRegion.z)
			|| (sFragmentCords.y < clippingRegion.y) || (sFragmentCords.y > clippingRegion.w))) {
			 
		gl_FragColor = highp vec4(1.0,0.0,0.0,0.0); //If outside the clipping region paint transparency
	}
	else if (textureEnabled >= 1.0) { 
		mediump vec4 texel = texture2D(basic_texture, pos,0.0);
		gl_FragColor = texel * uColor;
		//gl_FragColor = uColor;
		//gl_FragColor = vec4(pos.x,pos.y,basic_texture,1.0);
	}
	else { 
		gl_FragColor = uColor;
		//gl_FragColor = vec4(1.0,1.0,1.0,1.0);
	}
} 
  