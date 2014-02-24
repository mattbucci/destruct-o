
#pragma once

#include "Window.h"
#include "Button.h"

class Demo;

class DemoWindow : public Window {
	Button makePyramid;
	Button fireBlock;
	Button makeFire;
	Button increaseAcid;
	Button decreaseAcid;
public:
	DemoWindow(Demo * d);
	~DemoWindow();
};