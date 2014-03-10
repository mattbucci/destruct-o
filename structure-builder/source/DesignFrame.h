#pragma once

#include "GameSystem.h"
#include "GameCamera.h"
#include "Editor.h"
#include "Window.h"

class MaterialSelectionControl;

class DesignFrame : public GameSystem {
	Editor editor;

	bool panning;
	bool mouseDown;
	double mouseHeldAt;
	vec2 prevMousePos;

	MaterialSelectionControl * matSelector;
	Window matWindow;
	Structure * toEdit;
public:
	DesignFrame(ShaderGroup * shaders);
	~DesignFrame();



	//This is the main game object
	//so it contains all the subsystems
	GameCamera Camera;

	//for notes on Build() see GameSystem::Build()
	void Build() override;

	//Build basic entities (such as the player)
	void OnFrameFocus() override;

	//Override update to remove FPS control
	//provided by the game system
	virtual bool Update(double delta,double now, vector<InputEvent> inputEvents);

	//Draw happens whenever possible
	void Draw(double width, double height) override;
};