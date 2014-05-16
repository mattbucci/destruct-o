
#pragma once

#include "GameSystem.h"

class PlayerUpgradeMenu;

class UpgradeFrame : public GameSystem {
	PlayerUpgradeMenu * menu;
public:
	UpgradeFrame(ShaderGroup * shaders);
	~UpgradeFrame();


	//Build basic entities (such as the player)
	void OnFrameFocus() override;
};