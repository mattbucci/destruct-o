
#pragma once

#include "Control.h"
#include "Shape.h"

class MaterialSelectionControl : public Control {
	int currentlySelectedMaterial;

	Shape selectionShape;

protected:
	virtual void OnMousePress(vec2 mousePos, int button, bool down) override;

	

public:
	MaterialSelectionControl();
	MaterialSelectionControl(Rect position);
	~MaterialSelectionControl();

	virtual void Draw(GL2DProgram * shaders) override;

	int GetSelectedMaterial();
};