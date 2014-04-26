#include "stdafx.h"
#include "InterfaceGlobals.h"
#include "Font.h"
#include "NinePatchBinary.h"

//Defined in this cpp file
InterfaceGlobals VisualInterface;

void InterfaceGlobals::init() {
	//Setup the truetype system
	//Todo make constructor for TTF_Quit()
	TTF_Init();

	//Default colors
	ColorWindowBackground = vec4(.7,.7,.7,1);
	ColorWindowBorder = vec4(1,1,1,1);
	ColorControlBackground = vec4(.5,.5,.5,1);
	ColorControlSelection = vec4(.4,.4,.4,1);
	ColorControlHighlight = vec4(.6,.6,.6,1);
	ColorControlText = vec4(0,0,0,1);
	

	//Nine patches, overrides ugly colors
	NPWindow = new NinePatchBinary("Interface/9Patches/sample.9ptch");
	NPButton = NULL;
	NPButtonHover = NULL;
	NPButtonDown = NULL;
	NPListbox = NULL;
	NPTextbox = NULL;
	NPCombobox = NULL;
	//These will work in the future but don't right now
	NPListboxScrollbar = NULL;
	NPListboxUpArrow = NULL;
	NPListboxDownArrow = NULL;
	NPComboboxUpArrow = NULL;
	NPComboboxDownArrow = NULL;

	//Default Fonts
	FontWindowTitle = new Font("Interface/Fonts/times.ttf",30);
	FontControlText = new Font("Interface/Fonts/times.ttf",20);
}