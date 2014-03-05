#include "stdafx.h"
#include "LoadingScreen.h"
#include "lodepng.h"
#include "GL2DProgram.h"

LoadingScreen::LoadingScreen() : GL2DVertexGroup(GL_TRIANGLE_STRIP,4) {
	//Draw the classic square
	vat(0) = vec2(0,0);
	xat(0) = vec2(0,0);
	vat(1) = vec2(0,200);
	xat(1) = vec2(0,1);
	vat(2) = vec2(200,0);
	xat(2) = vec2(1,0);
	vat(3) = vec2(200,200);	
	xat(3) = vec2(1,1);

	//Load the texture
	unsigned int textureWidth, textureHeight;
	vector<unsigned char> image;
	unsigned error = lodepng::decode(image,textureWidth,textureHeight,"loading.png");
    if(error)
    {
        SDL_Log("Lodepng error (%u) - %s", error, lodepng_error_text(error));
    }
    
	//I should have moved the png->texture into a utility library
	//later...
	glGenTextures( 1, &textureId );
	glBindTexture(GL_TEXTURE_2D, textureId);


	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, textureWidth,
				textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
				&image[0] );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	

}
LoadingScreen::~LoadingScreen() {
	glDeleteTextures(1,&textureId);
}

//Draw the loading screen
void LoadingScreen::Draw(int width, int height, GL2DProgram * shader) {

	//Enable sensible defaults
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shader->UseProgram();
	//shader->SetWidthHeight(width,height);
	//Right now just stretch the loading screen
	shader->SetWidthHeight(200,200);
	shader->EnableTexture(true);
	glActiveTexture (GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,textureId);

	
	shader->Model.Reset();
	shader->Model.Apply();
	shader->Clipping.PushClippingRect(shader->Model,Rect(-1,-1,-1,-1));
	shader->SetColor(vec4(1,0,1,1));

	GL2DVertexGroup::Draw(shader);

	shader->Clipping.PopClippingRect();
}