
#pragma once

#include "Window.h"

class PictureButton;

class PlayerUpgradeMenu : public Window {
	//List the weapons the player can upgrade or unlock
	Window * weaponUpgradeMenu;
	PictureButton * weaponUpgradeChoices;

	struct weaponInformation {
		weaponInformation(int id, int cost, float costModifier, string name);
		//The name to be displayed
		string name;
		//the id used by the ActorPlayer
		int unlockId;
		//the cost to unlock this weapon
		int unlockCost;
		//The modifier to purchase upgrades to this weapon
		//modifies the default prices
		float upgradeCostModifier;
	};

	vector<weaponInformation> weapons;

public:
	PlayerUpgradeMenu();
	~PlayerUpgradeMenu();

	//Refresh with the latest information
	void Refresh();
};