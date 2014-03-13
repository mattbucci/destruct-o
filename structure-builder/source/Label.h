#pragma once

#include "Control.h"
#include "Font.h"
#include "RasterizedText.h"

class Label : public Control {
protected:
	void recalculatePosition() override;

	void applyWordwrap();
	//A wordwrap width max, if set to < 1 is ignored
	float maxWidth;
	//Wrap
	RasterizedText text;
public:
	Label();
	Label(float x, float y, string text);
	virtual ~Label();

	//Set max width for wordwrap (doesn't work right now)
	void SetMaxWidth(float maxWidth);

	//Draw label
	void Draw(GL2DProgram * shaders) override;

	//Wordwrap broken currently
	void SetText(string text);
	string GetText();

};