#include "stdafx.h"
#include "Label.h"
#include "Font.h"


Label::Label(float x, float y, string text) {
	position.X = x;
	position.Y = y;
	//Default to white
	color = vec4(1,1,1,0);

	this->text = RasterizedText(text,VisualInterface.FontControlText,VisualInterface.ColorControlText);
	//textSize = 20;
	maxWidth = 0;
}
Label::~Label() {

}

//Resize to fit text
void Label::recalculatePosition() {
	//Calculate width before you recalculate position
	vec2 size = text.Size();
	position.Width = size.x;
	position.Height = size.y;

	Control::recalculatePosition();
}

void Label::Draw(GL2DProgram * shaders) {
	//Draw text in black
	drawBackground(shaders);

	//Draw text
	text.Draw(shaders);

	drawChildren(shaders);
}

string Label::GetText() {
	return text.Text();
}

void Label::applyWordwrap() {
	//Wordwrap support broken with truetype fonts
	//will be added again in the future
	/*//No wordwrap to apply
	if (maxWidth < 1)
		return;
	//Apply wordwrap
	float lineHeight = 20;//textSize;
	string line = "";
	string word = "";
	float lineWidth = 0;

	string newText = "";

	for (int i = 0; i < text.length(); i++) {
		char c = text[i];
		if (c == ' ') {
			float wordWidth = activeFont.GetSizeString(word).x;
			if (wordWidth+lineWidth > maxWidth) {
				//TODO CHECK if line == "" so that long words are broken properly
				newText += "\n" + line;
				line = "";
				lineWidth = 0;
			}
			lineWidth += wordWidth;
			line += word + " ";
			word = "";
		}
		else if (c == '\n') {
			float wordWidth = activeFont.GetSizeString(word).x;
			//Check if the final word on the line fits on this line
			if (wordWidth+lineWidth > maxWidth)
				newText += "\n" + line + "\n  " + word;
			else {
				line += word + " ";
				newText += "\n" + line;
			}

			lineWidth = 0;
			line = "";
			word = "";
		}
		else
			word += c;
	}
	//Should the final word go inline or afterwards
	float wordWidth =  activeFont.GetSizeString(word).x;
	if (wordWidth+lineWidth > maxWidth) {
		newText += "\n" + line + "\n" + word;
		text = newText;
	}
	else {
		line += word + " ";
		newText += "\n" + line;
		text = newText;
	}

	*/
}

void Label::SetMaxWidth(float maxWidth) {
	this->maxWidth = maxWidth;
	//apply wordwrap
	applyWordwrap();
	//Recalculate label size instantly
	recalculatePosition();
}

void Label::SetText(string text) {
	//Only update if it changed
	if (this->text.Text() != text)
		this->text.SetText(text);
	//apply wordwrap
	applyWordwrap();
	//Recalculate label size instantly
	recalculatePosition();
}