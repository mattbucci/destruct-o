
#pragma once

#include "stdafx.h"
#include "InputEvent.h"
#include "ControlBase.h"
#include "GameEventSubscriber.h"

#include "FPSCounter.h"
#include "TextureCache.h"

class Sprite;
class ShaderGroup;

class GameSystem : protected GameEventSubscriber {
protected:
	FPSCounter fpsCount;
	set<Sint64> currentlyPressedKeys;
	ShaderGroup * shaders;

	//Handle key events, recording held keys and sending events to dialogs
	virtual void passEventsToControl(vector<InputEvent> & inputEvents);

	//For basic game system draw controls
	double nextFrameTime;
	int fps;
public:
	GameSystem(ShaderGroup * shaders);
	virtual ~GameSystem();

	//The root control of all windows displayed by this frame
	ControlBase Controls;

	//Texture cache system
	TextureCache Textures;

	//This is called right after all frames are constructed
	//but before any frames start running
	//it runs from a separate thread so do NOT do any opengl calls!
	//should be used for all heavy performance intensive loading
	//it is only called once
	virtual void Build();

	//When this frame starts running this will be called
	virtual void OnFrameFocus();
	//When this frame stops running this will be called
	virtual void OnFrameLeave();

	//When this frame is in focus these will happen
	//Update happens every SIMULATION_TIME  (.01s right now, which is 10ms)
	//actually simulation update doesn't happen at all right now
	virtual bool Update(double delta,double now, vector<InputEvent> inputEvents);
	//Draw happens whenever possible
	virtual void Draw(double width, double height);
}; 