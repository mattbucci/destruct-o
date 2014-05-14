#include "stdafx.h"
#include "Label.h"
#include "Font.h"


Label::Label() {
	//Default to white
	color = vec4(1,1,1,0);

	activeFont = VisualInterface.FontControlText;
	textColor = VisualInterface.ColorControlText;
	//textSize = 20;
	maxWidth = 0;
}

Label::Label(float x, float y, string text) {
	position.X = x;
	position.Y = y;
	//Default to white
	color = vec4(1,1,1,0);

	activeFont = VisualInterface.FontControlText;
	textColor = VisualInterface.ColorControlText;

	//textSize = 20;
	maxWidth = 0;

	//set text
	SetText(text);
}
Label::~Label() {

}

//Resize to fit text
void Label::recalculatePosition() {
	//Calculate width before you recalculate position
	position.Width = 0;
	position.Height = activeFont->GetFontSize()*textSegments.size();
	//The width is the longest segment
	for (auto segment : textSegments)
		position.Width = max(position.Width,segment.Size().x);

	Control::recalculatePosition();
}

void Label::Draw(GL2DProgram * shaders) {
	//Draw text in black
	drawBackground(shaders);

	//Draw text
	shaders->Model.PushMatrix();
	for (auto text : textSegments) {
		shaders->Model.Apply();
		//draw the text on this line
		text.Draw(shaders);
		//Move to the next line
		shaders->Model.Translate(0,activeFont->GetFontSize()*1.2f,0);
	}
	shaders->Model.PopMatrix();

	drawChildren(shaders);
}

string Label::GetText() {
	return originalText;
}


//Refresh the segments from wrapped text
void Label::refreshSegmentsFromWrappedText(vector<string> segments) {
	textSegments.resize(segments.size());
	//Now build appropriate segments for everything
	for (int i = 0; i < segments.size(); i++) {
		textSegments[i] = RasterizedText(segments[i],activeFont,textColor);
	}
}

void Label::applyWordwrap() {
	vector<string> segments;
	//Check if you can enabled wordwrap
	if (maxWidth >= 1) {
		//Apply wordwrap
		float lineHeight = 20;//textSize;
		string line = "";
		string word = "";
		float lineWidth = 0;

		string text = originalText;

		for (int i = 0; i < text.length(); i++) {
			char c = text[i];
			if (c == ' ') {
				float wordWidth = activeFont->GetSizeString(word).x;
				if (wordWidth+lineWidth > maxWidth) {
					//TODO CHECK if line == "" so that long words are broken properly
					segments.push_back(line);
					line = "";
					lineWidth = 0;
				}
				lineWidth += wordWidth;
				line += word + " ";
				word = "";
			}
			else if (c == '\n') {
				float wordWidth = activeFont->GetSizeString(word).x;
				//Check if the final word on the line fits on this line
				if (wordWidth+lineWidth > maxWidth) {
					segments.push_back(line);
					segments.push_back(word);
				}
				else {
					line += word + " ";
					segments.push_back(line);
				}

				lineWidth = 0;
				line = "";
				word = "";
			}
			else
				word += c;
		}
		//Should the final word go inline or afterwards
		float wordWidth =  activeFont->GetSizeString(word).x;
		if (wordWidth+lineWidth > maxWidth) {
			segments.push_back(line);
			segments.push_back(word);
		}
		else {
			line += word + " ";
			segments.push_back(line);
		}
	}
	else {
		//no wordwrap means use the original
		segments.push_back(originalText);
	}
	
	//Now segment the wrapped text
	refreshSegmentsFromWrappedText(segments);
}

void Label::SetMaxWidth(float maxWidth) {
	if (this->maxWidth == maxWidth)
		return;
	this->maxWidth = maxWidth;
	//apply wordwrap
	applyWordwrap();
	//Recalculate label size instantly
	recalculatePosition();
}

void Label::SetText(string text) {
	//Only update if it changed
	if (originalText == text)
		return;
	originalText = text;
	//apply wordwrap
	applyWordwrap();
	//Recalculate label size instantly
	recalculatePosition();
}

void Label::SetTextColor(vec4 color) {
	if (textColor == color)
		return;
	textColor = color;
	//apply wordwrap
	applyWordwrap();
	//Recalculate label size instantly
	recalculatePosition();
}