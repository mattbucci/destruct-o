#include "stdafx.h"
#include "Frames.h"
#include "OS.h"
#include "VoxEngine.h"
#include "AndroidGLEW.h"

#ifdef __ANDROID__ 

//Android's render loop is external, so this function is called from java
void doFrame(int width, int height) {
	VoxEngine::RenderLoop();
}
void VoxEngine::BuildRenderContext(){
	//Init nothing?
	SDL_Init(0);
	//Build an SDL window so we can get events
	SDL_CreateWindow("Sample",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN);


	//Use android glew to get all the necessary extensions
	//Let the android GLEW decide which opengl version to act like
	OpenglVersion = initAndroidGlew();
	if (OpenglVersion > 0)
		cout << "AndroidGLEW loaded successfully\n";
	else {
		cout << "Failed to load some necessary extensions for android glew\n";
		cout << "PROGRAM IS NOW EXITING\n";
		return;
	}
}


#endif