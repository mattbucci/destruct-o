#include "stdafx.h"
#include "Window.h"

#include "ControlBase.h"
#include "GL2DVertexGroup.h"
#include "NinePatchBinary.h"

Window::Window() {
	color = VisualInterface.ColorWindowBackground;
	useNinePatch(VisualInterface.NPWindow);
}
Window::Window(string title)  {
	this->title = RasterizedText(title,VisualInterface.FontWindowTitle,VisualInterface.ColorControlText);
	color = VisualInterface.ColorWindowBackground;
	useNinePatch(VisualInterface.NPWindow);
}
Window::Window(Rect position, string title) : Control(position)  {
	this->title = RasterizedText(title,VisualInterface.FontWindowTitle,VisualInterface.ColorControlText);
	color = VisualInterface.ColorWindowBackground;
	useNinePatch(VisualInterface.NPWindow);
}
Window::~Window() {

}

void Window::Draw(GL2DProgram * shaders) {
	drawBackground(shaders);

	drawChildren(shaders);

	//Center text
	shaders->Model.PushMatrix();
	shaders->Model.Translate(position.Width/2.0f-title.Size().x/2.0f,10,0);
	title.Draw(shaders);
	shaders->Model.PopMatrix();
}

//Add a control as a child of this window
void Window::AddControl(Control * controlToAdd) {
	Control::AddChild(controlToAdd);
}


void Window::SetTitle(string text) {
	title = RasterizedText(text,VisualInterface.FontWindowTitle,VisualInterface.ColorControlText);
}
string Window::GetTitle() {
	return title.Text();
}

bool Window::IsDialog() {
	return false;
}