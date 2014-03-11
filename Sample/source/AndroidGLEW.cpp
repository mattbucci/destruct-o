#include "stdafx.h"
#ifdef __ANDROID__
#include "AndroidGLEW.h"
#include <EGL/egl.h>



MYBIND_GLBINDVERTEXARRAY glBindVertexArray;
MYBIND_GLGENVERTEXARRAY glGenVertexArrays;
MYBIND_GLDELETEVERTEXARRAY glDeleteVertexArrays;

//Attempt to retrieve the linking of every requested GL function
#define SET(glFunctionName,functionType) if (!(glFunctionName = (functionType)eglGetProcAddress(#glFunctionName))) {return false;}

bool initAndroidGlew() {
	SET(glBindVertexArray,MYBIND_GLBINDVERTEXARRAY);
	SET(glGenVertexArrays,MYBIND_GLGENVERTEXARRAY);
	SET(glDeleteVertexArrays,MYBIND_GLDELETEVERTEXARRAY);

	//If you make it here the load was successful
	return true;
}
#endif