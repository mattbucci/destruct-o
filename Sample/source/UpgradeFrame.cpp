#include "stdafx.h"
#include "UpgradeFrame.h"
#include "PlayerUpgradeMenu.h"

UpgradeFrame::UpgradeFrame(ShaderGroup * shaders) : GameSystem(shaders) {
	menu = new PlayerUpgradeMenu();
	Controls.AddWindow(menu);
}
UpgradeFrame::~UpgradeFrame() {

}


//Build basic entities (such as the player)
void UpgradeFrame::OnFrameFocus() {
	menu->Refresh();
}