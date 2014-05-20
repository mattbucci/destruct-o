#include "stdafx.h"
#include "PauseFrame.h"
#include "GL2DProgram.h"
#include "GL3DProgram.h"
#include "Frames.h"
#include "BaseFrame.h"
#include "OS.h"

#include "PauseWindow.h"
#include "PauseWindowMobile.h"
#include "FirstPersonMode.h"



PauseFrame::PauseFrame(ShaderGroup * shaders)
    : GameSystem(shaders), background(Rect(0, 0, 800, 600),"menu/background.png", vec4(1,1,1,1)) 
{
#ifdef __MOBILE__
	pauseWindow = new PauseWindowMobile();
#else
	pauseWindow = new PauseWindow();
#endif
	Controls.AddWindow(pauseWindow);
	pauseWindow->SetVisible(true);
}

PauseFrame::~PauseFrame()
{

}

void PauseFrame::OnFrameFocus() {
}



bool PauseFrame::Update(vector<InputEvent> inputEvents) {
	//Pump events into the first person mode to find that escape key
	// Update the looking direction
	Game()->FirstPerson->ReadInput(currentlyPressedKeys,inputEvents);
    
    // Did we want to unpause
    if(Game()->FirstPerson->GetPauseRequested())
		Frames::SetSystem(Frames::FRAME_GAME);


	//Pass Update to GameSystem to Handle Control Input
	return GameSystem::Update(inputEvents);
}


void PauseFrame::Draw(double width, double height)
{
    float w = (float)width;
    float h = (float)height;
    //retrieve the texture
    GLTexture * texture = Textures.GetTexture<GLTexture>("menu/background.png");
    //Determine which dimension are constraints
    float widthRatio = w / texture->GetWidth();
    float heightRatio = h / texture->GetHeight();
    float aspectRatio = texture->GetWidth() / texture->GetHeight();
    float widthScale, heightScale;
    if (widthRatio > heightRatio) {
        //Stretch width to fit and size height appropriately
        widthScale = widthRatio;
        heightScale = widthScale / aspectRatio;
    }
    else {
        //stretch height to fit and size width appropriately
        heightScale = heightRatio;
        widthScale = heightScale*aspectRatio;
    }
    //Resize the image
    float imageWidth = widthScale * texture->GetWidth();
    float imageHeight = heightScale * texture->GetHeight();
    //Center the image
    Rect imageRect(w / 2.0f - imageWidth / 2.0f, h / 2.0f - imageHeight / 2.0f, imageWidth, imageHeight);
    background.SetRect(imageRect);

	GL2DProgram * shaders2d = SetWidthHeight((float)width, (float)height);

	shaders2d->Model.Reset();
	shaders2d->Model.Apply();
	background.Draw(shaders2d);
	shaders2d->Model.Translate((float)(width/2.0f - 648/2.0f), 80, 0);
	shaders2d->Model.Apply();
	shaders2d->Model.Reset();
	shaders2d->Model.Apply();

	//Call the parent draw to draw interface
	GameSystem::Draw(width, height);
}

