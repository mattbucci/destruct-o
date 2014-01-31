#pragma once

#include "Control.h"
#include "Shape.h"
#include "RasterizedText.h"

class Listbox : public Control {
	//The data contained in the listbox
	int selected;
	vector<RasterizedText> entryText;
	vector<string> entries;
	//Colors for the list box
	vec4 entryColor;
	vec4 backgroundColor;
	vec4 borderColor;
	vec4 selectedColor;
	//Colors for the scroll bar
	vec4 highlightColor;
	vec4 barColor;
	vec4 shapeColor;

	Shape selectionShape;
	Shape hoverShape;
	Shape backgroundShape;
	//Scrollbar
	Shape upShape;
	Shape downShape;
	Shape scrollPosShape;

	//Used for scrolling
	float scrollPosition;
	//If the button is being held, this was the last time
	//the scroll position was updated
	//if not held, = 0
	double lastScrollUpdate;
	vec2 lastMousePos;
	vec2 lastScrollPos; //Used to detect movement when dragging the scroll bar

	//Draws the scroll button in question
	//returns true if this button is being held down during draw
	bool drawScrollButton(GL2DProgram * shaders,bool isTop);
	//Draws the scroll bar automatically handling drag events and such
	void drawScrollBar(GL2DProgram * shaders);
	//Calculate the range of possible scrolling
	float scrollRange();
	//Calculate the distance the scroll bar can move
	float scrollBarRange();
	//Uses scroll position and width/height to calc scroll bar location/size
	Rect calcScrollBarRect();
	//Sets the scroll position within valid range
	void SafeScrollPosition();
protected:
	//Used to implement listbox functionality
	void Draw(GL2DProgram * shaders) override;
	void OnMousePress(vec2 mousePos, int button, bool down);
	void OnMouseMove(vec2 mousePos);
	void OnMouseLeave();
	void OnMouseEnter();
	void OnMouseScroll(bool scrolledUp);

	void OnListSelected(string selected);
public:
	Listbox();
	~Listbox();
	
	
	//Set the selected, or -1 for no selection
	void SetSelection(int selectionId);
	//Get the selection or -1 if nothing is selected
	int GetSelection();

	//Set or get the list of entries
	//entries are rasterized during Set() so avoid redundant calls
	void SetEntries(const vector<string> & entries);
	vector<string> GetEntries();

	//When the current selection changes
	//returns the string representing the selection
	function<void(string)> CBSelectionChanged;
};