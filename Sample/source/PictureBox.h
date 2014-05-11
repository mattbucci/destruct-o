#pragma once

#include "Control.h"

class PictureBox : public Control {
	string texture;
protected:
	void drawBackground(GL2DProgram * shaders) override;

public:
	PictureBox();
	PictureBox(Rect position);
	PictureBox(Rect position, string pictureTexture);


	void SetPicture(string texture);
	string GetPicture();
};