
#pragma once
#include "Control.h"
#include "Label.h"
#include "Shape.h"
#include "Font.h"
#include "RasterizedText.h"
#include "GameEvent.h"

class ComboBox : public Control {
 
	//True if the list of entries is being displayed
	//otherwise false
	bool opened;
	//Colors
	vec4 backColor; //Default color of the list
	vec4 hoverColor; //Color when user hovers cursor over entry or selected
	vec4 selectedColor;

	Label selected;
	vec2 lastMousePos;
	Shape hoverHighlight;
	Shape triangle;

	vector<string> entries;
	vector<RasterizedText> renderedEntries;
	string selectedEntry;
protected:
	virtual void Draw(GL2DProgram * shaders) override;
	virtual void OnMousePress(vec2 mousePos, int button, bool down) override;
	virtual void OnMouseMove(vec2 mousePos) override;
	virtual void OnMouseLeave() override;
	virtual void OnMouseEnter() override;
	//override to change height if opened
	virtual void recalculatePosition() override;

public:
	ComboBox(Rect size = Rect(0,0,100,100));
	~ComboBox();

	GameEvent<void(ComboBox*,int)> EventSelectionChanged;

	void AddComboEntry(string entry);
	void RemoveComboEntry(string entry);
	void AddList(vector<string> list);
	void SetSelected(string entry);
	void SetSelected(int selectedId);
	//GET
	vector<string> GetList();
	string GetSelected();
	int GetSelectedId();
};