#include "stdafx.h"
#include "BaseFrame.h"
#include "GL2DProgram.h"
#include "GL3DProgram.h"
#include "ShaderGroup.h"

#include "Window.h"
#include "Button.h"
#include "Listbox.h"

#include "OS.h"

BaseFrame::BaseFrame(ShaderGroup * shaders) : GameSystem(shaders) {
	//Build the dialog shader
	GL2DProgram * shaders2d = new GL2DProgram("Interface/Shaders/vsh_interface.glsl","Interface/Shaders/fsh_interface.glsl");
	if (!shaders2d->Valid()) 
		cout << "Failed to build opengl program\n";
	shaders->AddShader(shaders2d,"2d");
	//Build the voxel shader
	GL3DProgram * shaders3d = new GL3DProgram("Interface/Shaders/vsh_3d.glsl","Interface/Shaders/fsh_3d.glsl");
	if (!shaders3d->Valid()) 
		cout << "Failed to build opengl program\n";
	shaders->AddShader(shaders3d,"3d");

	//Load the sample tile
	if (!voxels.LoadTile("basic-h.png")) {
		cout << "Failed to load voxel tile\n";
	}

	//Build the sample dialog 
	//Build a window that says "On Top"
	Window * window = new Window(Rect(0,0,200,400),"On Bottom");
	window->hPin = Control::CENTER;
	window->vPin = Control::MAX;
	window->SetVisible(false);
	Controls.AddWindow(window);
	//Give it a list box with a bunch of random crap
	vector<string> randomCrap;
	randomCrap.push_back("Monkey");
	randomCrap.push_back("Walrus");
	randomCrap.push_back("Cheeto");
	randomCrap.push_back("Spaghetii");
	randomCrap.push_back("Canada");
	randomCrap.push_back("Banana");
	randomCrap.push_back("One");
	randomCrap.push_back("Seventeen");
	randomCrap.push_back("Lamp");
	randomCrap.push_back("Voxel");
	randomCrap.push_back("Game");
	randomCrap.push_back("Walrus");
	randomCrap.push_back("Still Walrus");
	randomCrap.push_back("Cloud");
	Listbox * list = new Listbox();
	list->position = Rect(0,0,180,150);
	list->hPin = list->vPin = Control::CENTER;
	list->SetEntries(randomCrap);
	//When the user selects something, change the title to the 
	//selected item
	list->CBSelectionChanged = [window](string changedTo) {
		window->SetTitle(changedTo);
	};
	window->AddControl(list);

	//Start another window (visible by default)
	Window * wm = new Window(Rect(0,0,300,300),"HELLO =)");
	wm->hPin = wm->vPin = Control::CENTER;
	Controls.AddWindow(wm);
	//Add a button to this window that when pressed
	//switches which window is visible
	Button * windowButton = new Button(Rect(0,-10,100,30),"PRESS ME");
	windowButton->hPin = Control::CENTER;
	windowButton->vPin = Control::MAX;
	windowButton->CBClicked = [window,wm]() {
		//Make the current window invisible
		wm->SetVisible(false);
		//Make the next window visible
		window->SetVisible(true);
	};
	wm->AddControl(windowButton);
	

}
BaseFrame::~BaseFrame() {

}

bool BaseFrame::Update(double delta,double now, vector<InputEvent> inputEvents) {
	//Issue events to dialog
	//Run the dialog system and monitor pressed keys
	passEventsToControl(inputEvents);

	return true;
}

void BaseFrame::Draw(double width, double height) {
	vec2 viewPortSize = vec2((float)width,(float)height);
	//Save size in camera as well (for unprojection)
	camera.UpdateViewSize(viewPortSize);
	vec2 mapExtents = vec2(voxels.GetWidth(),voxels.GetHeight());

	//Rotate camera
	camera.Rotation((float)((OS::Now()/30.0)*2.0f*M_PI));


	//New strategy for determining draw box
	//Create a rectangle with the user on the bottom center, with the top of the rectangle
	//far away from the user in his view direction
	//Use the points of this rotated rectangle
	//to choose a min/max point of the voxel draw rectangle


	//First determine visible extents by testing all corners
	viewPortSize = vec2(width,height);
	vec2 userPosition = vec2(camera.Position());
	float userAngle = camera.Rotation();
	static const float rectHalfWidth = 30.0f; //Half the width of the rectangle
	static const float rectHeight = 100.0f; //the full length of the rectangle
	static const float rectHalfDiagonal =(float)( M_PI/2.0f-atan2(rectHeight,rectHalfWidth)); //The angle of the diagonal (to the center of the width side)
	static const float rectDiagonalLength = sqrt(rectHalfWidth*rectHalfWidth+rectHeight*rectHeight);
	vec2 testPoints[4] = {
		//Use the edges of a rectangle with the viewer in the bottom center
		//First the bottom left
		userPosition + vec2(cos(M_PI/2.0f+userAngle),sin(M_PI/2.0f+userAngle))*rectHalfWidth,
		//Next bottom right
		userPosition + vec2(cos(-M_PI/2.0f+userAngle),sin(-M_PI/2.0f+userAngle))*rectHalfWidth,
		//Top Left
		userPosition + vec2(cos(rectHalfDiagonal+userAngle),sin(rectHalfDiagonal+userAngle))*rectDiagonalLength,
		//Top Right
		userPosition + vec2(cos(-rectHalfDiagonal+userAngle),sin(-rectHalfDiagonal+userAngle))*rectDiagonalLength,
	};
	vec2 minPoint = mapExtents;
	vec2 maxPoint = vec2(0,0);
	for (int i = 0; i < 4; i++) {
		minPoint = glm::min(minPoint,testPoints[i]);
		maxPoint = glm::max(maxPoint,testPoints[i]);
	}

	minPoint = glm::floor(minPoint);
	maxPoint = glm::ceil(maxPoint);
	//Limit points to valid ranges (vec2() creates a zero vector)
	minPoint = glm::max(vec2(),minPoint);
	maxPoint = glm::min(mapExtents-vec2(1,1),maxPoint);


	//Startup 3d rendering
	//Enable the 2d shader for interface drawing
	GL3DProgram * shaders3d = (GL3DProgram*)shaders->GetShader("3d");
	shaders3d->UseProgram();
	shaders3d->Model.Reset();
	shaders3d->Model.Apply();
	//Enable sensible defaults
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//Draw the frame
	//camera draw also sets up world light
	camera.Draw(shaders3d);
	voxels.Draw(shaders3d,camera.Position(),(int)minPoint.x,(int)minPoint.y,(int)maxPoint.x,(int)maxPoint.y);

	//Call the parent draw to draw interface
	//GameSystem::Draw(width,height);
}