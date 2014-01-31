#include "stdafx.h"
#include "Textbox.h"

#include "Font.h"
#include "OS.h"
#include "GameSystem.h"

Textbox::Textbox() : innerShape(4), selectionShape(4), textboxFont("times.ttf",20) {
	selectionShape.vset(0) = innerShape.vset(0) = vec2(0,0);
	selectionShape.vset(1) = innerShape.vset(1) = vec2(10,0);
	selectionShape.vset(2) = innerShape.vset(2) = vec2(0,10);
	selectionShape.vset(3) = innerShape.vset(3) = vec2(10,10);


	backColor = vec4(.3,.3,.3,1);
	borderColor = vec4(.8,.8,.8,1);
	textColor = vec4(1,1,1,1);

	//Starts at 25 chars
	maxTextLength = 25;

	AddChild(&innerShape);
	AddChild(&selectionShape);

	cursorPos = 0;
	useNinePatch(VisualInterface.NPTextbox);
}
Textbox::~Textbox() {

}

void Textbox::Draw(GL2DProgram * shaders) {
	color = borderColor;
	innerShape.color = backColor;
	innerShape.position.X = innerShape.position.Y = 3;
	innerShape.position.Width = position.Width-6;
	innerShape.position.Height = position.Height-6;

	//Selection not used right now
	//Size the selection shape to match the selection color
	selectionShape.position.Width = 2;
	selectionShape.position.Height = 20;
	vec2 size = textboxFont.GetSizeString(text.Text().substr(0,cursorPos));
	selectionShape.position.X = 8+size.x;
	selectionShape.position.Y = position.Height/2.0f - size.y/2.0f;
	
	if ((Control::getParent() != nullptr) && Control::getParent()->IsKeyboardFocused(this))
		//Blink the cursor
		selectionShape.SetVisible(((int)(OS::Now()*10) % 10) >= 5);
	else 
		selectionShape.SetVisible(false);


	//Draw the background
	drawBackground(shaders);

	//Draw the children of this control
	drawChildren(shaders);

	//Draw text in control
	
	//Center text
	shaders->Model.PushMatrix();
	//Center text vertically, but leave it left justified
	shaders->Model.Translate(10,position.Height/2.0f - 20/2.0f,0);
	text.Draw(shaders);
	shaders->Model.PopMatrix();
}

void Textbox::OnMousePress(vec2 mousePos, int button, bool down) {
	//Enable this control's focus
	if (down && (getParent() != nullptr))
		getParent()->ClaimKeyboardFocus(this);

	if (down) {
		//Move the text cursor over to an appropriate location
		string temp;
		string writtenText = text.Text();
		for (unsigned int i = 0; i <  writtenText.size(); i++) {
			temp +=  writtenText[i];
			if (mousePos.x < textboxFont.GetSizeString(temp).x) {
				cursorPos = i;
				return;
			}
		}
		//If you checked the whole string then just move the cursor to the end
		if (temp == writtenText)
			cursorPos =  writtenText.length();
	}
		
}
void Textbox::OnMouseMove(vec2 mousePos) {

}
void Textbox::OnMouseLeave() {

}
void Textbox::OnMouseEnter() {

}

void Textbox::OnTextChange(string newText) {
	if (CBTextChanged)
		CBTextChanged(newText);
}

void Textbox::OnKeyPress(int key) {
	string writtenText = text.Text();
	if (key == 25600)
		cursorPos--;
	else if (key == 26112)
		cursorPos++;
	else if (key == 8) {
		if (cursorPos > 0) {
			writtenText = writtenText.substr(0,cursorPos-1) + writtenText.substr(cursorPos,writtenText.size()-cursorPos);
			cursorPos--;
		}
	}
	else if (key == 127) {
		if (cursorPos < (int)writtenText.size())
			writtenText = writtenText.substr(0,cursorPos) + writtenText.substr(cursorPos+1,writtenText.size()-cursorPos);
	}
	else {
		//Only basic ansii characters are valid
		if ((key >= 32) && (key <= 126) && ((int)writtenText.size() < maxTextLength)) {
			writtenText = writtenText.substr(0,cursorPos) + (char)key + writtenText.substr(cursorPos,writtenText.size()-cursorPos);
			cursorPos++;
		}
			
	}

	OnTextChange(writtenText);

	//Keep the cursor between normal values
	if (cursorPos < 0)
		cursorPos = 0;
	if (cursorPos > (int)writtenText.size())
		cursorPos = writtenText.size();
}

void Textbox::SetText(string text) {
	this->text = RasterizedText(text,VisualInterface.FontControlText,VisualInterface.ColorControlText);
}
void Textbox::SetMaxLength(int length) {
	maxTextLength = length;
}
string Textbox::GetText() {
	return this->text.Text();
}