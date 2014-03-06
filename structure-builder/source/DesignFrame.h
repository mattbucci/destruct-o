#pragma once

#include "GameSystem.h"
#include "GameCamera.h"


class DesignFrame : public GameSystem {
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