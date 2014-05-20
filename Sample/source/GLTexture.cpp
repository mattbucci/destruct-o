#include "stdafx.h"
#include "GLTexture.h"
#include "lodepng.h"

//Default texture options
const GLTexture::textureFlags GLTexture::DefaultTextureOption  = GLTexture::TEXTURE_MIPMAP;

GLTexture::GLTexture(string texturePathName) {
	textureId = 0;
	this->texturePathName = texturePathName;
}
GLTexture::~GLTexture() {
	if (textureId > 0)
		glDeleteTextures(1,&textureId);
}

GLTexture * GLTexture::GenerateErrorTexture() {
	GLTexture * err = new GLTexture("");

	glGenTextures( 1, &err->textureId );

	glBindTexture(GL_TEXTURE_2D, err->textureId);

	//texture mapping type, perhaps elsewhere?
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );	//GL_NEAREST FOR SPEED
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	//GL_NEAREST FOR SPEED
    
	vector<vec4> errorTexture(8*8);
	//Generate a red/green 8x8 pattern which is the error pattern
	for (int x = 0; x < 8; x++) {
		bool red = ((x % 2) == 0);
		for (int y = 0; y < 8; y++) {
			red = !red;
			if (red)
				errorTexture[x*8+y] = vec4(1,0,0,1);
			else
				errorTexture[x*8+y] = vec4(0,1,0,1);
		}
	}

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_FLOAT, &errorTexture[0] );
	return err;
}




//stolen from http://www.geeksforgeeks.org/write-one-line-c-function-to-find-whether-a-no-is-power-of-two/
/* Function to check if x is power of 2*/
static bool isPowerOfTwo (int x) {
  /* First x in the below expression is for the case when x is 0 */
  return x && (!(x&(x-1)));
}

//Attempt to cache the texture in graphics memory
bool GLTexture::CacheTexture(textureFlags flags){
	//Check that you're not already cached
	if (IsCached())
		return true;

	vector<unsigned char>  data;
	unsigned error = lodepng::decode(data, width, height, texturePathName);

	if(error) {
		cout << "Failed to cache texture " << texturePathName << "\n";
		return false;
	}

	
	glGenTextures( 1, &textureId );

	glBindTexture(GL_TEXTURE_2D, textureId);

	//texture mapping type, perhaps elsewhere?
	if ((flags & TEXTURE_MIPMAP) && isPowerOfTwo(width) && isPowerOfTwo(height)) {
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
    
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width,
					height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
					&data[0] );

		//Decreases load performance substantially. Should be fixed later
		glGenerateMipmap(GL_TEXTURE_2D);		
	}
	else if (flags & TEXTURE_LINEAR) {
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );	//GL_NEAREST FOR SPEED
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_NEAREST FOR SPEED
    
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width,
					height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
					&data[0] );
	}
	else {
		///asume fastest
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );	//GL_NEAREST FOR SPEED
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	//GL_NEAREST FOR SPEED
    
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width,
					height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
					&data[0] );
	}


	return textureId > 0;
}

//Whether or not this texture is cached currently
bool GLTexture::IsCached() {
	return textureId > 0;
}

float GLTexture::GetWidth() {
	return (float)width;
}
float GLTexture::GetHeight() {
	return (float)height;
}

void GLTexture::Bind() {
	glBindTexture(GL_TEXTURE_2D,textureId);
}