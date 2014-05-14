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

	vec4 textColor;

	//rasterized texts
	//as generated for the wrappedText
	vector<RasterizedText> textSegments;
	

	//100% original text with no word wrapping
	string originalText;

	//The font used by this Label
	Font * activeFont;

	//Refresh the segments from wrapped text
	void refreshSegmentsFromWrappedText(vector<string> segments);
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
	void SetTextColor(vec4 color);
	string GetText();

};