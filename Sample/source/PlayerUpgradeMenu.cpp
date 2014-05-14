#include "stdafx.h"
#include "PlayerUpgradeMenu.h"
#include "PictureButton.h"


//All the weapon information should go here

PlayerUpgradeMenu::weaponInformation::weaponInformation(int id, int cost, float costModifier, string name) {
	unlockId = id;
	unlockCost = cost;
	upgradeCostModifier = costModifier;
	this->name = name;
}


PlayerUpgradeMenu::PlayerUpgradeMenu() {
	//Setup the weapon information for all weapons
	weapons.push_back(weaponInformation(0,0,1.0f,"Laser Cannon"));
	weapons.push_back(weaponInformation(1,200,1.5f,"Pulse Laser"));

	//Setup this window
	position = Rect(0,0,650,550);
	hPin = vPin = Control::CENTER;
}
PlayerUpgradeMenu::~PlayerUpgradeMenu()  {

}

//Refresh with the latest information
void PlayerUpgradeMenu::Refresh() {

}