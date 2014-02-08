#pragma once

#include "stdafx.h"

class Font;
class NinePatchBinary;

class InterfaceGlobals {
	friend class VoxEngine;
	//Build all the defaults
	void init();

public:
	//Default colors
	vec4 ColorWindowBackground;
	vec4 ColorWindowBorder;
	vec4 ColorControlBackground;
	vec4 ColorControlForeground;
	vec4 ColorControlSelection;
	vec4 ColorControlHighlight;
	vec4 ColorControlText;

	//Nine patches, overrides ugly colors
	NinePatchBinary * NPWindow;
	NinePatchBinary * NPButton;
	NinePatchBinary * NPButtonHover;
	NinePatchBinary * NPButtonDown;
	NinePatchBinary * NPListbox;
	NinePatchBinary * NPTextbox;
	NinePatchBinary * NPCombobox;
	//These will work in the future but don't right now
	NinePatchBinary * NPListboxScrollbar;
	NinePatchBinary * NPListboxUpArrow;
	NinePatchBinary * NPListboxDownArrow;
	NinePatchBinary * NPComboboxUpArrow;
	NinePatchBinary * NPComboboxDownArrow;
	
	//Default Fonts
	Font * FontWindowTitle;
	Font * FontControlText;
};

//Global containing the defaults for the interface
extern InterfaceGlobals VisualInterface;