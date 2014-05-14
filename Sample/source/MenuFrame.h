#pragma once

#include "VoxEngine.h"
#include "ShaderGroup.h"
#include "GameSystem.h"
#include "Window.h"
#include "Button.h"
#include "TexturedRect.h"

class MenuFrame : public GameSystem {
	TexturedRect background;
	TexturedRect logo;

	Window buttonWindow;

	Button playButton;
	Button loadButton;
	Button optionsButton;

public:
	MenuFrame(ShaderGroup * shaders);
	~MenuFrame();


	//Build basic entities (such as the player)
	void OnFrameFocus() override;

	//Override update to remove FPS control
	//provided by the game system
	virtual bool Update(vector<InputEvent> inputEvents) override;

	//Draw happens whenever possible
	void Draw(double width, double height) override;

};
