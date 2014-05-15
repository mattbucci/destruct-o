#include "stdafx.h"
#include "PlayerUpgradeMenu.h"
#include "PictureButton.h"
#include <sstream>

#include "VoxEngine.h"
#include "ActorPlayer.h"
#include "BaseFrame.h"

#include "Frames.h"


//All the weapon information should go here

PlayerUpgradeMenu::weaponInformation::weaponInformation(int id, int cost, float costModifier, string name, string lockedImage, string unlockedImage) {
	unlockId = id;
	unlockCost = cost;
	upgradeCostModifier = costModifier;
	this->name = name;
	this->lockedImage = string("Interface/UI/") + lockedImage;
	this->unlockedImage = string("Interface/UI/") + unlockedImage;
}


PlayerUpgradeMenu::PlayerUpgradeMenu() {
	//Setup the weapon information for all weapons
	weapons.push_back(weaponInformation(0,0,1.0f,"Laser Cannon","placeholder.png","placeholder.png"));
	weapons.push_back(weaponInformation(1,200,1.5f,"Pulse Laser","placeholder.png","placeholder.png"));



	//Create all the weapon upgrade choices
	weaponUpgradeChoices = new PictureButton[weapons.size()];
	//Move them into appropriate locations
	for (int i = 0; i < weapons.size(); i++) {
		AddChild(weaponUpgradeChoices+i);
		//Setup defaults
		weaponUpgradeChoices[i].position = Rect(20+i*220,10,200,200);
		weaponUpgradeChoices[i].vPin = Control::CENTER;
		weaponUpgradeChoices[i].SetPicturePosition(Rect(.3,.5,.4,.4));
		weaponUpgradeChoices[i].SetTextPosition(.25);
		//Subscribe
		Subscribe<void(Button*)>(&weaponUpgradeChoices[i].EventClicked, [this,i](Button * b) {
			//Get the player's points
			int & points = VoxEngine::SavedAccountData.GameAchievements.PointsUnspent;
			//Get the weapon modifier
			WeaponModifiers * m = Game()->Actors.Player()->GetModifiers(weapons[i].unlockId);

			if (m->DamageFactor <= 0) {
				//Weapon is locked, and the player is trying to unlock it, if they have enough money unlock it
				if (points >= weapons[i].unlockCost) {
					//They can afford it
					points -= weapons[i].unlockCost;
					//Unlock the weapon
					m->DamageFactor = 1.0f;
					//Play some kind of notification or something?
					//Update UI
					this->Refresh();
				}
			}
			else {
				//They want to upgrade the weapon
				//open the uprade dialog
			}

		});
	}
	//Back Button
	Button * backButton = new Button(Rect(-20,10,130,200));
	backButton->hPin = Control::MAX;
	backButton->vPin = Control::CENTER;
	backButton->SetText("Back");
	Subscribe<void(Button*)>(&backButton->EventClicked, [this](Button * b) {
		Frames::SetSystem(Frames::FRAME_GAME);
	});
	AddChild(backButton);


	currentScore = new Label(0,10,"");
	currentScore->hPin = Control::CENTER;
	AddChild(currentScore);

	//Setup this window
	position = Rect(0,0,600,260);
	hPin = vPin = Control::CENTER;
}
PlayerUpgradeMenu::~PlayerUpgradeMenu()  {

}

//Refresh with the latest information
void PlayerUpgradeMenu::Refresh() {
	//Update spent/unspent
	stringstream ss;
	int points = VoxEngine::SavedAccountData.GameAchievements.PointsUnspent;
	ss << "You have " <<  points << " to spend";
	currentScore->SetText(ss.str());
	ss.str("");

	//Update the weapon display
	for (int i = 0; i < weapons.size(); i++) {
		WeaponModifiers * m = Game()->Actors.Player()->GetModifiers(weapons[i].unlockId);
		//Check if the weapon is unlocked
		if (m->DamageFactor <= 0) {
			//Weapon is locked
			ss << "Unlock " << weapons[i].name << " - " << weapons[i].unlockCost << "pts";
			weaponUpgradeChoices[i].SetTexture(weapons[i].lockedImage);
			weaponUpgradeChoices[i].SetTextColor((points >= weapons[i].unlockCost) ? vec4(0,1,0,1) : vec4(0,0,1,1));
		}
		else {
			//Weapon is unlocked
			ss << "Upgrade " << weapons[i].name;
			weaponUpgradeChoices[i].SetTexture(weapons[i].unlockedImage);
			weaponUpgradeChoices[i].SetTextColor(vec4(1,1,1,1));
		}
		//set weapon upgrade text
		weaponUpgradeChoices[i].SetText(ss.str());
		
		ss.str("");
	}
}