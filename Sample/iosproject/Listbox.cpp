#include "stdafx.h"
#include "Listbox.h"

#include "OS.h"
#include "GL2DProgram.h"
#include "Font.h"

static const float entryHeight = 20;
static const float scrollButtonWidth = 20;

bool Listbox::drawScrollButton(GL2DProgram * shaders,bool isTop) {
	Rect drawPos;
	Shape * triangleShape;
	vec4 drawColor;
	bool retValue = false;

	//First select values based off of button
	if (isTop) {
		triangleShape = &upShape;
		drawPos = Rect(position.Width-3-scrollButtonWidth,3,scrollButtonWidth,scrollButtonWidth);
	}
	else {
		triangleShape = &downShape;
		drawPos = Rect(position.Width-3-scrollButtonWidth,position.Height-3-scrollButtonWidth,scrollButtonWidth,scrollButtonWidth);
	}
	//Now select color based off of cursor status
	if (drawPos.InRect(lastMousePos)) {
		if (lastScrollUpdate != 0) {
			retValue = true;
			drawColor = highlightColor;
		}
		else
			drawColor = (highlightColor+barColor)/2.0;
	}
	else
		drawColor = barColor;

	//Calculate the triangle position
	Rect tPos = Rect(drawPos.Width*.25f,drawPos.Height*.25f,drawPos.Width*.5f,drawPos.Height*.5f);

	//Now draw the shape
	shaders->Model.PushMatrix();
	shaders->Model.Translate(drawPos.X,drawPos.Y,0);

	scrollPosShape.color = drawColor;
	scrollPosShape.OverrideCalculatedSize(drawPos);
	scrollPosShape.Draw(shaders);

	//Draw the triangle too
	shaders->Model.Translate(tPos.X,tPos.Y,0);
	triangleShape->color = shapeColor;
	triangleShape->OverrideCalculatedSize(tPos);
	triangleShape->Draw(shaders);

	shaders->Model.PopMatrix();

	return retValue;
}

//Calculate the range of possible scrolling
float Listbox::scrollRange() {
	return entryHeight*entries.size() - (position.Height-6);
}
//Calculate the distance the scroll bar can move
float Listbox::scrollBarRange() {
	return position.Height-scrollButtonWidth*4-6;
}

Rect Listbox::calcScrollBarRect() {
	Rect s = Rect(0,0,scrollButtonWidth,scrollButtonWidth*2);
	s.X = position.Width-scrollButtonWidth-3;
	//The scroll position is between 0 and scrollRange() so the
	//scroll bar must be between 0 and its range
	s.Y = 3+scrollButtonWidth+scrollBarRange()*-scrollPosition/scrollRange();
	return s;
}

void Listbox::SafeScrollPosition() {
	if (scrollPosition > 0)
		scrollPosition = 0;
	float scrollMax = scrollRange();
	if (scrollPosition < -scrollMax)
		scrollPosition = -scrollMax;
}

void Listbox::drawScrollBar(GL2DProgram * shaders) {
	vec4 drawColor;
	Rect scrollBarLoc = calcScrollBarRect();
	//Is the user dragging the bar?
	if ((lastScrollUpdate != 0) && (lastScrollPos.x >= 0)) {
		//The bar is being dragged now
		drawColor = highlightColor;
		//Determine the new scrollPosition scrollPosition =
		float scrollBarMovement = (lastScrollPos.y-lastMousePos.y)/scrollBarRange();
		scrollPosition += scrollBarMovement*scrollRange();
		lastScrollPos.y = lastMousePos.y;

		//Limit scroll position
		SafeScrollPosition();

		//Since the scroll position moved, recalculate the bars position
		scrollBarLoc = calcScrollBarRect();
	}
	else if (scrollBarLoc.InRect(lastMousePos)) {
		if (lastScrollUpdate != 0) {
			//Just started dragging
			drawColor = highlightColor;
			lastScrollPos = lastMousePos;
		}
		else
			drawColor = (highlightColor+barColor)/2.0;
	}
	else
		drawColor = barColor;
	//Finally draw the bar
	shaders->Model.PushMatrix();
	shaders->Model.Translate(scrollBarLoc.X,scrollBarLoc.Y,0);

	scrollPosShape.color = drawColor;
	scrollPosShape.OverrideCalculatedSize(scrollBarLoc);
	scrollPosShape.Draw(shaders);

	shaders->Model.PopMatrix();
}

//Used to implement listbox functionality
void Listbox::Draw(GL2DProgram * shaders) {
	//Check selected
	if (selected >= (int)entries.size())
		selected = -1;


	//Calculate if a scroll bar is needed
	bool scrollBarNeeded = (entryHeight*entries.size() > position.Height-6);


	//Set inherited properties
	color = borderColor;
	selectionShape.color = selectedColor;
	selectionShape.position.Width = position.Width-6;
	selectionShape.position.Height = entryHeight;
	selectionShape.position.X = 3;
	backgroundShape.color = backgroundColor;
	backgroundShape.position.Width = position.Width-6;
	backgroundShape.position.Height = position.Height-6;
	backgroundShape.position.X = 3;
	backgroundShape.position.Y = 3;
	hoverShape.color = vec4(selectedColor.x,selectedColor.y,selectedColor.z,.5);
	hoverShape.position.Width = position.Width-6;
	hoverShape.position.Height = entryHeight;
	hoverShape.position.X = 3;

	if (scrollBarNeeded) {
		//Make room for the scroll bar
		selectionShape.position.Width -= scrollButtonWidth;
		hoverShape.position.Width -= scrollButtonWidth;		
	}

	//Draw the border using the background of this control
	drawBackground(shaders);

	//draw the background and selections using children
	if ((selected < 0) || (selected >= (int)entries.size()))
		selectionShape.SetVisible(false);
	else {
		selectionShape.position.Y = scrollPosition+3+entryHeight*selected;
		selectionShape.SetVisible(true);
	}
	if (lastMousePos.x >= 3) {
		//Mouse is over control
		int hoveringOver = (int)((lastMousePos.y-scrollPosition-3)/entryHeight);

		//Check that the cursor is not in the scroll bar
		if (scrollBarNeeded) {
			if (lastMousePos.x >= calculatedPosition.Width-3-scrollButtonWidth)
				hoveringOver = -1;
		}

		if ((hoveringOver < 0) || (hoveringOver >= (int)entries.size()))
			hoverShape.SetVisible(false);
		else {
			hoverShape.position.Y = scrollPosition+3+entryHeight*hoveringOver;
			hoverShape.SetVisible(true);
		}
	}
	else
		hoverShape.SetVisible(false);



	//Add an additional clipping region to protect the border
	shaders->Clipping.PushClippingRect(shaders->Model,backgroundShape.position);
	//Draw the children of this control
	drawChildren(shaders);

	//If the scroll bar is needed protect it as well
	if (scrollBarNeeded) {
		//We use a new clipping region because we wanted to draw the background
		//but we don't want to draw the text in this region
		shaders->Clipping.PopClippingRect();
		shaders->Clipping.PushClippingRect(shaders->Model,Rect(backgroundShape.position.X,backgroundShape.position.Y,
			backgroundShape.position.Width-scrollButtonWidth,backgroundShape.position.Height));
	}


	//Draw all the entries
	//Center text
	shaders->Model.PushMatrix();
	//Center text vertically, but leave it left justified
	shaders->Model.Translate(10,3+scrollPosition,0);
	for (auto entry : entryText) {
		entry.Draw(shaders);
		shaders->Model.Translate(0,entryHeight,0);
	}
	shaders->Model.PopMatrix();
	//Remove the additional clipping region
	shaders->Clipping.PopClippingRect();

	//Draw the scroll bar if needed
	if (scrollBarNeeded) {
		//First draw top and bottom scroll buttons
		if (drawScrollButton(shaders,true)) {
			double now = OS::Now();
			scrollPosition += 100*(float)(now-lastScrollUpdate);
			lastScrollUpdate = now;
		}
		if (drawScrollButton(shaders,false)) {
			double now = OS::Now();
			scrollPosition -= 100*(float)(now-lastScrollUpdate);
			lastScrollUpdate = now;
		}
		//Next draw/process bar
		drawScrollBar(shaders);

		//Force scroll position within valid ranges
		SafeScrollPosition();
	}
}
void Listbox::OnMousePress(vec2 mousePos, int button, bool down) {
	if (down)
		lastScrollUpdate = OS::Now();
	else {
		lastScrollUpdate = 0;
		lastScrollPos = vec2(-1,-1);
		//Mouse up, check if they selected something
		//Use the hovering over calculation stolen from the draw phase
		int hoveringOver = (int)((lastMousePos.y-scrollPosition-3)/entryHeight);

		//Check that the cursor is not in the scroll bar
		bool scrollBarNeeded = (entryHeight*entries.size() > position.Height-6);
		if (scrollBarNeeded) {
			if (lastMousePos.x >= calculatedPosition.Width-3-scrollButtonWidth)
				hoveringOver = -1;
		}

		if ((hoveringOver < 0) || (hoveringOver >= (int)entries.size()))
			return;
		else {
			selected = hoveringOver;
			OnListSelected();
		}
			
	}
		
}

void Listbox::OnListSelected() {
	EventSelectionChanged.Fire([this](function<void(Listbox*,int)> subscriber) {
		subscriber(this,this->selected);
	});
}

void Listbox::OnMouseMove(vec2 mousePos) {
	lastMousePos = mousePos;
}
void Listbox::OnMouseLeave() {
	lastMousePos = vec2(-1,-1);
	lastScrollUpdate = 0;
	lastScrollPos = vec2(-1,-1);
}
void Listbox::OnMouseEnter() {

}
void Listbox::OnMouseScroll(bool scrolledUp) {
	if (scrolledUp)
		scrollPosition += entryHeight/2.0f;
	else
		scrollPosition -= entryHeight/2.0f;

	//Keep scroll position within allowed values
	SafeScrollPosition();
}

Listbox::Listbox() : 
	selectionShape(4), hoverShape(4), backgroundShape(4),
	upShape(3), downShape(3), scrollPosShape(4) {

	//Setup default colors
	entryColor = vec4(1,1,1,1);
	backgroundColor = vec4(.2,.2,.2,1);
	borderColor = vec4(1,1,1,1);
	selectedColor = VisualInterface.ColorControlSelection;
	//Setup default scroll bar colors
	highlightColor = VisualInterface.ColorControlHighlight;
	barColor = vec4(.4,.4,.4,1);
	shapeColor = vec4(0,0,0,1);
	//Make shapes square
	selectionShape.SetToSquare();
	backgroundShape.SetToSquare();
	hoverShape.SetToSquare();
	upShape.SetToUpTriangle();
	downShape.SetToDownTriangle();
	scrollPosShape.SetToSquare();


	//Add all the shapes as children
	AddChild(&backgroundShape);
	AddChild(&selectionShape);
	AddChild(&hoverShape);
	//Scroll bar shapes are manually drawn and aren't true children

	//Set initial values for scrolling
	lastMousePos = vec2(-1,-1);
	lastScrollPos = vec2(-1,-1);
	scrollPosition = 0;
	lastScrollUpdate = 0;
	selected = -1;

	useNinePatch(VisualInterface.NPListbox);
}
Listbox::~Listbox() {

}

//Set the selected, or -1 for no selection
void Listbox::SetSelection(int selectionId) {
	//validate legal selection id
	_ASSERTE(selectionId > -2);
	_ASSERTE(selectionId < (int)entries.size());
	//set the selection
	selected = selectionId;
}
int Listbox::GetSelection() {
	return selected;
}

//Set or get the list of entries
//entries are rasterized during Set() so avoid redundant calls
void Listbox::SetEntries(const vector<string> & entries) {
	this->entries = entries;
	entryText.clear();
	//Rasterize the text for drawing
	for (const string & entry : entries) 
		entryText.push_back(RasterizedText(entry,VisualInterface.FontControlText,VisualInterface.ColorControlText));
	//Invalidate the current selection
	selected = -1;
}
vector<string> Listbox::GetEntries() {
	return entries;
}