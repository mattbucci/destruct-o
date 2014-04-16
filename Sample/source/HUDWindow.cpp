#include "stdafx.h"
#include "HUDWindow.h"


HUDWindow::HUDWindow(void)
{
}

HUDWindow::HUDWindow(BaseFrame* parent) {
	this->parent = parent;

	position = Rect(0, 30, 800, 570);
	color = vec4(1.0f, 1.0f, 1.0f, 0.5f);
	takeInput = false;
}

HUDWindow::~HUDWindow(void)
{
}


void HUDWindow::Draw(GL2DProgram * shaders) {
	//drawBackground(shaders);
	drawChildren(shaders);

	glBegin(GL_QUADS);
	glColor3b(255, 255, 255);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glVertex2f(0.0f, 0.0f);
	glVertex2f(0.0f, 10.0f);
	glVertex2f(10.0f, 10.0f);
	glVertex2f(10.0f, 0.0f);
	glPopMatrix();
	glEnd();
}