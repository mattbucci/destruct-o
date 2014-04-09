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

		//This is not actually a mode, but it used to track the size of the mode list
		//it must always be at the end
		MODE_LAST
	};
private:
	//Draw any rect or region the editor needs
	class Region : GLDynamicVertexGroup {
	public:
		string Texture;

		Region(vec3 cornerA, vec3 cornerB, string textureName);
		void Resize(vec3 cornerA, vec3 cornerB, bool renderTop = true);
		void Draw(GL3DProgram * shader, vec3 pos);
	};

	//Trace the given ray to the floor or sides of the editor extents
	void traceToConstructionBox(pair<vec3,vec3> ray, vec3 & rayCollision, vec3 & surfaceNormal);

	//Calculate the desired cursor position based off of the given cursor position
	//updates selectedVoxel
	void UpdateCurrentVoxel(vec2 cursorPosition);

	//Used to render structure voxels and editor hints
	VoxelDrawSystem * drawSystem;
	//The current mode of the editor
	//determines how input is used to find the selected voxel
	EditorMode mode;
	
	//The currently selected voxel
	bool selectionValid;
	vec3 selectedVoxel;

	//The current level for LEVELBYLEVEL
	int currentLevel;

	//The material of the next voxel you are going to place
	int materialId;

	//The structure being modified by the editor
	Structure * beingEdited;

	//A reference to the camera is necessary to properly translate screen space to 3d space
	GameCamera * camera;

	//Whether or not the editor is currently enabled
	bool enabled;

	//The last two voxel positions in which a voxel was placed
	//used for creating rectangles
	bool placedVoxelOneLast;
	vec3 lastVoxelOne;
	vec3 lastVoxelTwo;

	Region drawFloor;
	Region drawVoxel;
	Region drawArea;
public:
	Editor(GameCamera * camera);

	//Whether or not the editor is currently enabled
	//a disabled editor will not react to input or draw anything
	void EnableEditor(bool enable);

	//Specify the structure to edit
	//and the size of one side of the square (starts at 0,0 and goes to size,size)
	//do not specify NULL
	void EditStructure(Structure * structure);

	//Read input not consumed by the dialog system
	void ReadInput(vector<InputEvent> input);

	//Retrieve the editor mode
	EditorMode GetMode();

	//Set the new editor mode
	void SetMode(EditorMode newMode);

	//change the current material id 
	void SetMaterial(int materialId);

	//The user has requested the editor place a voxel at the current position
	void PlaceVoxel();

	//Attempt to construct a cube between the last two placed voxels
	void PlaceCuboid();

	//Draw the editor guidelines  
	void Draw(GL3DProgram * shader);
};