#include "stdafx.h"
#include "Control.h"
#include "GL2DProgram.h"

#include "GameSystem.h"
#include "NinePatchBinary.h"

	//Get the control owning this control
Control * Control::getParent() {
	return parent;
}
//register a child to this control
void Control::AddChild(Control * child) {
	children.push_back(child);
	child->parent = this;
	//If the child can receive keyboard events sign it up now
	if (child->textControl)
		keyboardFocusedChild = child;
	//If the child has a child which can receive keyboard events sign that up instead
	else if (child->keyboardFocusedChild != nullptr)
		keyboardFocusedChild = child->keyboardFocusedChild;
}

void Control::OnMousePress(vec2 mousePos, int button, bool down) {
	//localPos -= vec2(calculatedPosition.X,calculatedPosition.Y);

	//If there's a focused child, send the click to that child
	if (focusedChild != NULL)
		focusedChild->OnMousePress(mousePos-vec2(focusedChild->calculatedPosition.X,focusedChild->calculatedPosition.Y),button,down);

	//Iterate through the list of children to check for mouse preview
	for (Control* & child : children) {
		//Must be visible to receive events
		if (!child->visible)
			continue;

		if (child->requestMousePreview && child != focusedChild) {
			child->OnMousePress(mousePos-vec2(child->calculatedPosition.X,child->calculatedPosition.Y),button,down);
		}
	}
}

bool Control::hasFocusedChild() {
	return focusedChild != NULL && ((Window*)focusedChild)->takeInput;
}

void Control::OnMouseScroll(bool scrolledUp) {
	//Doesn't support mouse preview currently
	if (focusedChild != NULL)
		focusedChild->OnMouseScroll(scrolledUp);
}

void Control::OnMouseMove(vec2 mousePos) {

	//Iterate through the list of children to check for mouse preview
	for (Control* & child : children) {
		//Must be visible to receive events
		if (!child->visible)
			continue;

		if (child->requestMousePreview) {
			child->OnMouseMove(mousePos-vec2(child->calculatedPosition.X,child->calculatedPosition.Y));
		}
	}

	//Children must be iterated in reverse so the click goes to the control on top
	//that is, the control which draws last and on top of all others
	for (int i = children.size()-1; i >= 0; i--) {
		Control * child = children[i];
		//Must be visible to receive events
		if (!child->visible)
			continue;

		if (child->calculatedPosition.InRect(mousePos)) {
			//If the control the cursor is in isn't the previously
			//focused child, change the focused child
			if (child != focusedChild) {
				if (focusedChild)
					focusedChild->OnMouseLeave();
				focusedChild = child;
				focusedChild->OnMouseEnter();
			}
			//If the child has requestMousePreview then the event has already been sent once. Do not send again
			if (!focusedChild->requestMousePreview)
				focusedChild->OnMouseMove(mousePos-vec2(child->calculatedPosition.X,child->calculatedPosition.Y));

			//Cursor can only be in one child at a time
			return;
		}
	}
	//Not hovering over any child anymore
	if (focusedChild != NULL) {
		focusedChild->OnMouseLeave();
		focusedChild = NULL;
	}

}
void Control::OnMouseLeave() {
	if (focusedChild != NULL) {
		focusedChild->OnMouseLeave();
		focusedChild = NULL;
	}
}
void Control::OnMouseEnter() {
	//This is a side-effect and does not have to be passed up
}

void Control::OnKeyPress(int key) {
	//Only called if this control is in focus 
}

void Control::__OnKeyPress(int key) {
	OnKeyPress(key);
}


Control::Control(int vertexCount) :
	GL2DVertexGroup(GL_TRIANGLE_STRIP,vertexCount) {

	position.Width = position.Height = 100;
	color = VisualInterface.ColorControlBackground;
	visible = true;
	parent = NULL;
	requestMousePreview = false;

	//"pinned" to the minimum by default
	vPin=hPin=MIN;
	keyboardFocusedChild = NULL;
	textControl = false;
	ninePatchToDraw = NULL;

	focusedChild = NULL;
}
Control::Control(Rect sizeAndPos,int vertexCount) :
	GL2DVertexGroup(GL_TRIANGLE_STRIP,vertexCount){

	position = sizeAndPos;
	color = VisualInterface.ColorControlBackground;
	visible = true;
	parent = NULL;
	requestMousePreview = false;

	//"pinned" to the minimum by default
	vPin=hPin=MIN;
	keyboardFocusedChild = NULL;
	textControl = false;
	ninePatchToDraw = NULL;

	focusedChild = NULL;
}

void Control::RemoveInvalidClaim() {
	keyboardFocusedChild = NULL;
	if ((parent != NULL) && (parent->keyboardFocusedChild != NULL))
		parent->RemoveInvalidClaim();
}

void Control::ClaimKeyboardFocus(Control * childControl) {
	//If the claim is NULL, travel downwards until all descendants also know their claim has been nullified
	if (childControl == nullptr) {
		for (auto child : children)
			if (child->keyboardFocusedChild != NULL)
				child->ClaimKeyboardFocus(NULL);
		return;
	}
	//If there was a previous person who claimed it, mark their claim as void
	if ((keyboardFocusedChild != nullptr) && (keyboardFocusedChild != childControl)) {
		ClaimKeyboardFocus(nullptr);
	} 
	//If anyone claims keyboard control, the claim travels upwards until only one control has keyboard control
	if (parent != nullptr)
		parent->ClaimKeyboardFocus(childControl);
	//Set the claim
	keyboardFocusedChild = childControl;
}

Control::~Control() {
	//If this control has a parent, unregister yourself as a child
	if (parent != NULL) {
		//parent->children.erase(this);
		for (auto it = parent->children.begin(); it != parent->children.end(); it++) {
			if (*it == this) {
				parent->children.erase(it);
				break;
			}
		}
		//If you're your parent's focused child, destroy the focused child
		if (parent->focusedChild == this)
			parent->focusedChild = NULL;

		if (parent->keyboardFocusedChild == this)
			parent->keyboardFocusedChild = NULL;
		
	}

	//Copy the children array, since children automatically unregister themselves
	//your array will be modified automatically so only iterate over a copy of it
	vector<Control*> childrenArray = children;
	//Destroy all the children of this control
	for (Control* & child : childrenArray)
		delete child;
}

void Control::drawChildren(GL2DProgram * shaders) {
	for (Control* & child : children) {
		if (!child->visible)
			continue;
		
		//Calculate the size of the child
		child->recalculatePosition();

		//Apply the child's region as a clipping region so the child can't draw into the parent
		shaders->Clipping.PushClippingRect(shaders->Model,child->calculatedPosition);

		//Draw the child in the inner rect
		shaders->Model.PushMatrix();
		shaders->Model.Translate(child->calculatedPosition.X,child->calculatedPosition.Y,0);
		child->Draw(shaders);
		shaders->Model.PopMatrix();

		shaders->Clipping.PopClippingRect();
	}
}

bool Control::IsKeyboardFocused(Control * childControl) {
	return (keyboardFocusedChild == childControl);
}

//draw this control before all others
void Control::SendToFront() {
	if (parent != NULL) {
		/*if (this == parent->children[0])
			//Already in front
			return;*/

		//locate the index this control is at in the parent
		int originalIndex = -1;
		for (unsigned int i = 0; i < parent->children.size(); i++) {
			if (parent->children[i] == this) {
				originalIndex = i;
				break;
			}
		}
		//Use an insertion sort to move this child to the front
		for (unsigned int i = originalIndex; i < parent->children.size()-1; i++) 
			parent->children[i] = parent->children[i+1];
		
		parent->children[parent->children.size()-1] = this;
	}
}
//draw this control last
void Control::SendToBack() {
	if (parent != NULL) {
		//locate the index this control is at in the parent
		int originalIndex = -1;
		for (unsigned int i = 0; i < parent->children.size(); i++) {
			if (parent->children[i] == this) {
				originalIndex = i;
				break;
			}
		}
		//Move to draw last so that this control is last
		Control * previousFront = parent->children[0];
		parent->children[0] = parent->children[originalIndex];
		parent->children[originalIndex] = previousFront;
	}
}

void Control::drawBackground(GL2DProgram * shaders) {
	shaders->Model.Apply();
	//Adjust vertices for the background
	if (ninePatchToDraw == NULL) {
		//Draw the classic square
		vat(0) = vec2(0,0);
		vat(1) = vec2(0,calculatedPosition.Height);
		vat(2) = vec2(calculatedPosition.Width,0);
		vat(3) = vec2(calculatedPosition.Width,calculatedPosition.Height);	
		//Enable pure-color no texture
		shaders->SetColor(color);
		shaders->EnableTexture(false);
	}
	else {
		//Alias the ninepatch to make this next part easier to type
		NinePatchBinary * n = ninePatchToDraw;

		//Determine the scaled width and height based off of the normal width,height
		float scaledWidth = position.Width - ((n->TotalSize().x - (n->MWidth() + n->MLeft())) + n->MLeft());
		float scaledHeight = position.Height - ((n->TotalSize().y - (n->MHeight() + n->MTop())) + n->MTop());

		//Draw the textured nine patch
		//by resizing the texture coordinates
		//using the 9patch system
		//Coordinates arranged like this:
		//A = 0, B = 1, C = 2, etc.
		//A  C  E  G  
		//BN DL FJ H 
		//O  MQ KS IU
		//P  R  T  V
		vat(0) = vec2(0,0);
		vat(1) = vec2(0,n->MTop());
		vat(2) = vec2(n->MLeft(),0);
		vat(3) = vec2(n->MLeft(),n->MTop());
		vat(4) = vec2(n->MLeft()+scaledWidth,0);
		vat(5) = vec2(n->MLeft()+scaledWidth,n->MTop());
		vat(6) = vec2(position.Width,0);
		vat(7) = vec2(position.Width,n->MTop());
		vat(8) = vec2(position.Width,n->MTop()+scaledHeight);
		vat(9) = vat(5);
		vat(10) = vec2(n->MLeft()+scaledWidth,n->MTop()+scaledHeight);
		vat(11) = vat(3);
		vat(12) = vec2(n->MLeft(),n->MTop()+scaledHeight);
		vat(13) = vat(1);
		vat(14) = vec2(0,n->MTop()+scaledHeight);
		vat(15) = vec2(0,position.Height);
		vat(16) = vat(12);
		vat(17) = vec2(n->MLeft(),position.Height);
		vat(18) = vat(10);
		vat(19) = vec2(n->MLeft()+scaledWidth,position.Height);
		vat(20) = vat(8);
		vat(21) = vec2(position.Width,position.Height);
		//Whew that was a nightmare
		shaders->SetColor(vec4(1,1,1,1));
		//Set the nine patch texture
		n->Bind9Texture(shaders);
	}

	GL2DVertexGroup::Draw(shaders);
}

float Control::findPinPos(Control::PinType pin, float mySize, float parentMax) {
	switch (pin) {
	case Control::MIN:
		return 0;
	case Control::CENTER:
		return (parentMax - mySize)/2.0f;
	case Control::MAX:
		return parentMax - mySize;
	}
	return 0; //not pinned
}

void Control::recalculatePosition() {
	//Width is the same but height is different
	calculatedPosition.Width = position.Width;
	calculatedPosition.Height = position.Height;

	//Attempt to calculate X,Y from parent and pin
	if (parent != NULL) {
		calculatedPosition.X = position.X + findPinPos(hPin, position.Width, parent->position.Width);
		calculatedPosition.Y = position.Y + findPinPos(vPin, position.Height, parent->position.Height);
	}
	else {
		calculatedPosition.X = position.X;
		calculatedPosition.Y = position.Y;
	}
}


bool Control::GetVisible() {
	return visible;
}
void Control::SetVisible(bool visible) {
	if (!visible) {
		//Remove the keyboard claim going upwards
		ClaimKeyboardFocus(NULL);
		//remove the keyboard claim going downwards
		RemoveInvalidClaim();
	}
	else if (parent != nullptr) {
		//Bring yourself to the top
		SendToFront();
	}
		
	this->visible = visible;
}

void Control::Draw(GL2DProgram * shaders) {
	//Draw the background
	drawBackground(shaders);

	//Draw the children of this control
	drawChildren(shaders);

}



void Control::useNinePatch(NinePatchBinary * ninePatch) {
	if ((ninePatch == NULL) || (!ninePatch->IsValid())) {
		//Must be a valid nine patch to continue
		//Reset to not using a ninepatch
		ResizeVertexGroup(4);
		this->ninePatchToDraw = NULL;
		return;
	}


	this->ninePatchToDraw = ninePatch;
	//Resize to the 4x4 system the ninepatch uses
	//Requires some extra points
	ResizeVertexGroup(22);
	vector<vec2> ninePatchCoordinates = ninePatch->GetTextureCoordinates();
	//Copy the texture coordinates now
	for (int i = 0; i < 22; i++)
		xat(i) = ninePatchCoordinates[i];
}