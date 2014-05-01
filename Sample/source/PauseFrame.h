#pragma once

#include "VoxEngine.h"
#include "ShaderGroup.h"
#include "GameSystem.h"
#include "TexturedRect.h"

class PauseWindow;

class PauseFrame : public GameSystem {
	TexturedRect background;

	PauseWindow * pauseWindow;

public:
	PauseFrame(ShaderGroup * shaders);
	~PauseFrame();


	//Build basic entities (such as the player)
	void OnFrameFocus() override;

	//Override update to remove FPS control
	//provided by the game system
	virtual bool Update(vector<InputEvent> inputEvents) override;


	//Draw happens whenever possible
	void Draw(double width, double height) override;

};
