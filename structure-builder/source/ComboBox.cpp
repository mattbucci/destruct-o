#include "stdafx.h"
#include "ComboBox.h"
#include "Font.h"

void ComboBox::recalculatePosition() {
	Control::recalculatePosition();
	if (opened) {
		//Change your height to encompass the full list
		calculatedPosition.Height *= 1+entries.size();
	}
}

void ComboBox::Draw(GL2DProgram * shaders) {
	color = backColor;
	
	//Build the hover highlight
	hoverHighlight.color = hoverColor;
	hoverHighlight.position.Width = position.Width;
	hoverHighlight.position.Height = position.Height;


	if (opened) {
		//Make the triangle face up
		triangle.SetToUpTriangle();

		if (lastMousePos.x > 0) {
			//Check which item is highlighted
			hoverHighlight.SetVisible(true);
			hoverHighlight.position.Y = position.Height*(int)(lastMousePos.y/position.Height);
		}
		else
			hoverHighlight.SetVisible(false);
	}
	else  {
		//Make the triangle face down
		triangle.SetToDownTriangle();
		if (lastMousePos.x >= 0) {
			hoverHighlight.SetVisible(true);
			hoverHighlight.position.Y = 0;
		}
		else
			hoverHighlight.SetVisible(false);
	}
	drawBackground(shaders);

	//Set properties of the label to match the properties of this object
	selected.SetText(selectedEntry);
	drawChildren(shaders);

	//Draw the labels on top of children
	if (opened) {
		//Center text
		shaders->Model.PushMatrix();
		//Center text vertically, but leave it left justified
		shaders->Model.Translate(10,(float)(position.Height*1.5-20*.5),0);
		for (auto entry : renderedEntries) {
			entry.Draw(shaders);
			shaders->Model.Translate(0,position.Height,0);
		}
		shaders->Model.PopMatrix();
	}
}


void ComboBox::OnMousePress(vec2 mousePos, int button, bool down) {
	if (!down)
		return;

	if (opened) {
		int selected = (int)(lastMousePos.y/position.Height);
		if (selected > 0) {
			selectedEntry = entries[selected-1];
			EventSelectionChanged.Fire([this,selected](function<void(ComboBox*,int)> member){
				member(this,selected);
			});
		}
	}
	
	opened = !opened;

	//Send to front so the drop down is on top
	if (opened) {
		sendToFront();
	}
		
}
void ComboBox::OnMouseMove(vec2 mousePos) {
	lastMousePos = mousePos;
}
void ComboBox::OnMouseLeave() {
	//The mouse is now out of the control
	//don't highlight anything
	lastMousePos = vec2(-1,-1);
}
void ComboBox::OnMouseEnter() {

}

ComboBox::ComboBox(float x, float y, float width, float height) :
	Control(Rect(x,y,width,height)),
	selected(0,0,""),
	hoverHighlight(4),
	triangle(3) {
	//Not opened by default
	opened = false;

	//Set the default colors
	backColor = VisualInterface.ColorControlBackground;
	hoverColor = VisualInterface.ColorControlHighlight;
	selectedColor = VisualInterface.ColorControlSelection;
	
	selected.hPin = Control::CENTER;
	selected.vPin = Control::CENTER;

	triangle.color = vec4(0,0,0,1);
	triangle.vset(0) = vec2(10,0);
	triangle.vset(1) = vec2(20,20);
	triangle.vset(2) = vec2(0,20);
	hoverHighlight.vset(0) = vec2(0,0);
	hoverHighlight.vset(1) = vec2(10,0);
	hoverHighlight.vset(2) = vec2(0,10);
	hoverHighlight.vset(3) = vec2(10,10);

	triangle.hPin = Control::MAX;
	triangle.vPin = Control::CENTER;
	triangle.position.Width = triangle.position.Height = 15;
	triangle.position.X = -10;
	//triangle.position.X = -20;


	AddChild(&hoverHighlight);
	AddChild(&triangle);
	AddChild(&selected);

	//Mouse not in control by default
	lastMousePos = vec2(-1,-1);

	useNinePatch(VisualInterface.NPCombobox);
}
ComboBox::~ComboBox() {

}

//vector<string> entries;

void ComboBox::AddComboEntry(string entry) {
	entries.push_back(entry);
}
void ComboBox::RemoveComboEntry(string entry) {
	string e = entry;
	for (auto it = entries.begin(); it != entries.end(); it++) {
		//Delete only the first entry that matches the given description
		if (*it == e) {
			entries.erase(it);
			return;
		}
	}
}
void ComboBox::AddList(vector<string> list) {
	string entry;
	//Add every entry stopping at the first nil value
	entries = list;
}
void ComboBox::SetSelected(string entry) {
	selectedEntry = entry;
}
vector<string> ComboBox::GetList() {
	return entries;
}
string ComboBox::GetSelected() {
	return selectedEntry;
}
void ComboBox::SetSelected(int selectedId) {
	selectedEntry = entries[selectedId];
}

int ComboBox::GetSelectedId() {
	int index = 0;
	for (string & s : entries) {
		if (s == selectedEntry)
			return index;
		index++;
	}
	return -1;
}