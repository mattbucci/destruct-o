#pragma once

#include "stdafx.h"
#include "Rect.h"
#include "GL2DVertexGroup.h"
#include "InterfaceGlobals.h"

class GL2DProgram;
class NinePatchBinary;

class Control : protected GL2DVertexGroup {
public:
	//Pin this control to a position within its parent
	enum PinType {
		MIN,
		CENTER,
		MAX
	};
private:
	Control * parent;
	//The child in which the mouse is over right now
	Control * focusedChild;

	//Pass the OnKeyPress to the control. Used by ControlBase
	void __OnKeyPress(int key);
	//Control can be visible or not visible
	bool visible;
	//Remove invalidated keyboard focus claim
	void RemoveInvalidClaim();
	//The Controlbase is a friend class so that it can issue events
	friend class ControlBase;

	//Is NULL unless this control is using a nine patch
	NinePatchBinary * ninePatchToDraw;
protected:
	//True if this control can receive keyboard events
	//defaults to false
	bool textControl;
	//Returns true if focusedChild != NULL
	bool hasFocusedChild();
	vector<Control*> children;

	//Get the control owning this control
	Control * getParent();
	//register a child to this control
	void AddChild(Control * child);

	//Events that can pass through this control
	virtual void OnMousePress(vec2 mousePos, int button, bool down);
	virtual void OnMouseMove(vec2 mousePos);
	virtual void OnMouseLeave();
	virtual void OnMouseEnter();
	virtual void OnMouseScroll(bool scrolledUp);
	//OnKeyPress automatically repeats
	//Check OnKeyDown and OnKeyUp if you want more refined control
	virtual void OnKeyPress(int key);

	//used for drawing
	bool textureEnabled;

	//draw this control before all others
	void sendToFront();
	//draw this control last
	void sendToBack();

	//Attempts to set the control to use a ninepatch
	//if the ninepatch is not valid, does nothing
	//the pointer must remain valid for the life of this control
	//This will malfunction terribly with certain controls
	//which expect there to be only 4 vertices
	//Check that the draw function does not expect this
	//before making this call
	void useNinePatch(NinePatchBinary * ninePatch);

	//Draws the children of this control
	virtual void drawChildren(GL2DProgram * shaders);
	//Draws the background of this control IF this control is only 4 vertex
	virtual void drawBackground(GL2DProgram * shaders);
	//Construct calculatedPosition from position,pin,and parent
	//Automatically called before draw()
	virtual void recalculatePosition();

	//A child which now has keyboard focus 
	Control * keyboardFocusedChild;

	//Determine offset position from a pin
	float findPinPos(PinType pin, float myWidth, float parentMax);

	//Recalculated every draw
	Rect calculatedPosition;

	//A control with "requestMousePreview" will receive mouse events from the parent
	//even if the mouse events are not over that control
	bool requestMousePreview;
public:
	Control(int vertexCount = 4);
	Control(Rect sizeAndPos,int vertexCount = 4);

	//An object's position effects is used to add to the calculated PinType 
	PinType vPin;
	PinType hPin;

	//A destroyed control kills its children
	virtual ~Control();

	//Position/size of control
	//Depends on pin
	Rect position;

	//Background color
	vec4 color;

	bool GetVisible();
	void SetVisible(bool visible);

	//set a particular child control as the control currently in keyboard focus
	void ClaimKeyboardFocus(Control * childControl);
	//Check if a particualr child is currently in keyboard focus
	bool IsKeyboardFocused(Control * childControl);

	virtual void Draw(GL2DProgram * shaders);

};