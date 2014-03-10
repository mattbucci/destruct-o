#include "stdafx.h"
#include "Editor.h"
#include "GameSystem.h"
#include "GL3DProgram.h"
#include "Frames.h"
#include "Structure.h"

Editor::Region::Region(vec3 cornerA, vec3 cornerB, string textureName) {
	UpdateGroup(GL_TRIANGLES,36);
	Resize(cornerA,cornerB);
}
void Editor::Region::Resize(vec3 cornerA, vec3 cornerB) {
	int vertex = 0;

	//Top
	pushSide(vec3(0,0,1),vec3(0,0,1),vec3(0,1,1),vec3(1,0,1),vec3(1,1,1),vertex);
	//Bottom
	pushSide(vec3(0,0,-1),vec3(0,0,0),vec3(0,1,0),vec3(1,0,0),vec3(1,1,0),vertex);
	//Left
	pushSide(vec3(1,0,0),vec3(1,0,0),vec3(1,1,0),vec3(1,0,1),vec3(1,1,1),vertex);
	//Right
	pushSide(vec3(-1,0,0),vec3(0,0,0),vec3(0,1,0),vec3(0,0,1),vec3(0,1,1),vertex);
	//Back
	pushSide(vec3(0,1,0),vec3(0,1,0),vec3(0,1,1),vec3(1,1,0),vec3(1,1,1),vertex);
	//Front
	pushSide(vec3(0,-1,0),vec3(0,0,0),vec3(1,0,0),vec3(0,0,1),vec3(1,0,1),vertex);

	//Go through all the sides and apply the corners so instead of 0,0,0 to 1,1,1 it's from cornerA, to cornerB
	for (int i = 0; i < vertex; i++) {
		//X coordinate
		if (vat(i).x < .001)
			vat(i).x = cornerA.x;
		else
			vat(i).x = cornerB.x;
		//Y coordinate
		if (vat(i).y < .001)
			vat(i).y = cornerA.y;
		else
			vat(i).y = cornerB.y;
		//Z coordinate
		if (vat(i).z < .001)
			vat(i).z = cornerA.z;
		else
			vat(i).z = cornerB.z;
	}
	//cout << "{" << 
}


void Editor::Region::pushSide(vec3 normal, vec3 a, vec3 b, vec3 c, vec3 d, int & vertNumber) {
	xat(vertNumber) = vec2(0,0);
	nat(vertNumber) = normal;
	vat(vertNumber++) = vec3(a);

	xat(vertNumber) = vec2(1,0);
	nat(vertNumber) = normal;
	vat(vertNumber++) = vec3(b);

	xat(vertNumber) = vec2(0,1);
	nat(vertNumber) = normal;
	vat(vertNumber++) = vec3(c);

	xat(vertNumber) = vec2(1,0);
	nat(vertNumber) = normal;
	vat(vertNumber++) = vec3(b);

	xat(vertNumber) = vec2(1,1);
	nat(vertNumber) = normal;
	vat(vertNumber++) = vec3(d);

	xat(vertNumber) = vec2(0,1);
	nat(vertNumber) = normal;
	vat(vertNumber++) = vec3(c);
}

void Editor::Region::Draw(GL3DProgram * shader, vec3 pos) {
	//Apply the position
	shader->Model.PushMatrix();
	shader->Model.Translate(pos);
	shader->Model.Apply();

	//Apply the texture
	CurrentSystem->Textures.GetTexture(texture)->Bind();
	//Draw the region
	GLDynamicVertexGroup::Draw(shader);

	shader->Model.PopMatrix();
}

Editor::Editor(GameCamera * camera) : drawFloor(vec3(),vec3(),"highlight.png"),
	drawVoxel(vec3(),vec3(1,1,1),"highlight.png"),
	drawArea(vec3(),vec3(),"highlight.png") {

	SetMode(MODE_STICKY);
	this->camera = camera;
	beingEdited = NULL;
	drawSystem = VoxelDrawSystem::BuildAppropriateSystem();
};

void Editor::UpdateCurrentVoxel(vec2 cursorPosition) {
	switch (mode) {
	case MODE_STICKY: {
		pair<vec3,vec3> ray = camera->Unproject(cursorPosition);
		vec3 voxelHit;
		vec3 voxelNormal;
		//Trace to the structure
		if (beingEdited->EditorTraceToVoxel(ray.first,ray.second,voxelHit,voxelNormal)) {
			//In sticky mode modify the voxel being hovered over by the normal
			voxelHit += voxelNormal;
			selectedVoxel = voxelHit;
		}
		else
			//Failing that trace to the ground
			selectedVoxel = vec3(camera->UnprojectToGround(cursorPosition,0.0f),0.0f);

		selectedVoxel = glm::floor(selectedVoxel);
	}
		break;
	case MODE_LEVELBYLEVEL: {
		//Trace to the current level
		selectedVoxel = vec3(camera->UnprojectToGround(cursorPosition,currentLevel),currentLevel);
		selectedVoxel = glm::floor(selectedVoxel);
	}
		break;
	case MODE_BUILDUP: {
		pair<vec3,vec3> ray = camera->Unproject(cursorPosition);
		vec3 voxelHit;
		vec3 voxelNormal;
		//Trace to the structure
		if (beingEdited->EditorTraceToVoxel(ray.first,ray.second,voxelHit,voxelNormal)) {
			//In sticky mode modify the voxel being hovered over by the normal
			voxelHit += voxelNormal;
			selectedVoxel = voxelHit;
		}
		else
			//Failing that trace to the ground
			selectedVoxel = vec3(camera->UnprojectToGround(cursorPosition,0.0f),0.0f);

		selectedVoxel = glm::floor(selectedVoxel);

		//MODE_BUILDUP works exactly like MODE_STICKY until this point
		//at this point disregard the x,y and find the highest point to place 
		//the voxel in that x,y
		vec3 highestPoint = vec3(selectedVoxel.x,selectedVoxel.y,0.0f);
		for (int i = 0; i < beingEdited->Cells.size(); i++) {
			vec3 pos = beingEdited->Cells[i].pos;
			if ((pos.x == selectedVoxel.x) || (pos.y == selectedVoxel.y)) {
				if (highestPoint.z < pos.z)
					highestPoint.z = pos.z;
			}
		}
		//Now we want to be 1 above the highest point
		selectedVoxel = highestPoint + vec3(0,0,1);
	}
		break;
	}
	//Now limit the selectedVoxel to the permitted region

	cout << "<" << selectedVoxel.x << "," << selectedVoxel.y << "," << selectedVoxel.z << ">\n";
}

//Specify the structure to edit
//and the size of one side of the square (starts at 0,0 and goes to size,size)
void Editor::EditStructure(Structure * structure,int size) {
	beingEdited = structure;
	width = height = size;
	drawArea.Resize(vec3(),vec3((float)size,(float)size,20.0f));
}

//Read input not consumed by the dialog system
void Editor::ReadInput(vector<InputEvent> input) {
	for (auto e : input) {
		//Process mouse movements
		if (e.Event == InputEvent::MouseMove)
			UpdateCurrentVoxel(vec2(e.MouseX,e.MouseY));
	}
}

//Retrieve the editor mode
Editor::EditorMode Editor::GetMode() {
	return mode;
}

//Set the new editor mode
void Editor::SetMode(EditorMode newMode) {
	mode = newMode;
	currentLevel = 0;
}

//Draw the editor guidelines  
void Editor::Draw(GL3DProgram * shader) {
	_ASSERTE(beingEdited);
	//Draw the structure
	beingEdited->EditorRenderStructure(shader,drawSystem);
	//Draw transparent objects
	drawVoxel.Draw(shader,vec3());
}