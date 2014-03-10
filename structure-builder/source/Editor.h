#pragma once

#include "stdafx.h"
#include "VoxelDrawSystem.h"
#include "InputEvent.h"
#include "GLDynamicVertexGroup.h"
#include "GameCamera.h"

class GL3DProgram;
class Structure;

class Editor {
public:
	enum EditorMode {
		//Select level you're working on and place voxels anywhere on that 2d level (up/down arrows control level)
		MODE_LEVELBYLEVEL,
		//place voxels on top of any existing voxels or floor (ray trace to voxel, place voxel over ray traced area)
		MODE_BUILDUP,
		//Place voxels next to surface mouse is hovering over
		MODE_STICKY,
		//Remove the voxel the cursor is over
		MODE_DELETE,
	};
private:
	//Draw any rect or region the editor needs
	class Region : GLDynamicVertexGroup {
		//add a side to this region
		void pushSide(vec3 normal, vec3 a, vec3 b, vec3 c, vec3 d, int & vertNumber);
		string texture;
	public:
		Region(vec3 cornerA, vec3 cornerB, string textureName);
		void Resize(vec3 cornerA, vec3 cornerB);
		void Draw(GL3DProgram * shader, vec3 pos);
	};

	//Calculate the desired cursor position based off of the given cursor position
	//updates selectedVoxel
	void UpdateCurrentVoxel(vec2 cursorPosition);

	//Used to render structure voxels and editor hints
	VoxelDrawSystem * drawSystem;
	//The current mode of the editor
	//determines how input is used to find the selected voxel
	EditorMode mode;
	
	//The currently selected voxel
	vec3 selectedVoxel;

	//The current level for LEVELBYLEVEL
	int currentLevel;

	//The size of the drawable area
	int width;
	int height;

	//The structure being modified by the editor
	Structure * beingEdited;

	//A reference to the camera is necessary to properly translate screen space to 3d space
	GameCamera * camera;

	Region drawFloor;
	Region drawVoxel;
	Region drawArea;
public:
	Editor(GameCamera * camera);

	//Specify the structure to edit
	//and the size of one side of the square (starts at 0,0 and goes to size,size)
	void EditStructure(Structure * structure,int size);

	//Read input not consumed by the dialog system
	void ReadInput(vector<InputEvent> input);

	//Retrieve the editor mode
	EditorMode GetMode();

	//Set the new editor mode
	void SetMode(EditorMode newMode);

	//Draw the editor guidelines  
	void Draw(GL3DProgram * shader);
};