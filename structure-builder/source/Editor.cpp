#include "stdafx.h"
#include "Editor.h"
#include "GameSystem.h"
#include "GL3DProgram.h"
#include "Frames.h"
#include "Structure.h"

Editor::Region::Region(vec3 cornerA, vec3 cornerB, string textureName) {
	UpdateGroup(GL_TRIANGLES,36);
	Resize(cornerA,cornerB);
	Texture = textureName;
}
void Editor::Region::Resize(vec3 cornerA, vec3 cornerB) {
	int vertex = 0;

	//Top
	pushSide(vec3(0,0,1),vec3(0,0,1),vec3(1,0,1),vec3(0,1,1),vec3(1,1,1),vertex);
	//Bottom
	pushSide(vec3(0,0,-1),vec3(0,0,0),vec3(0,1,0),vec3(1,0,0),vec3(1,1,0),vertex);
	//Left
	pushSide(vec3(1,0,0),vec3(1,0,0),vec3(1,1,0),vec3(1,0,1),vec3(1,1,1),vertex);
	//Right
	pushSide(vec3(-1,0,0),vec3(0,0,0),vec3(0,0,1),vec3(0,1,0),vec3(0,1,1),vertex);
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
		//cout << "{" << vat(i).x << "," << vat(i).y << "," << vat(i).z << "}\n";
	}
	//cout << "";
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
	CurrentSystem->Textures.GetTexture(Texture)->Bind();
	
	//Draw the region
	GLDynamicVertexGroup::Draw(shader);

	shader->Model.PopMatrix();
}

Editor::Editor(GameCamera * camera) : drawFloor(vec3(),vec3(),"Interface/drawArea.png"),
	drawVoxel(vec3(),vec3(1.1,1.1,1.1),"Interface/drawVoxel.png"),
	drawArea(vec3(),vec3(),"Interface/drawFloor.png") {

	SetMode(MODE_STICKY);
	this->camera = camera;
	beingEdited = NULL;
	drawSystem = VoxelDrawSystem::BuildAppropriateSystem();
	selectionValid = false;
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
		vec3 highestPoint = vec3(selectedVoxel.x,selectedVoxel.y,-1.0f);
		for (int i = 0; i < beingEdited->Cells.size(); i++) {
			vec3 pos = beingEdited->Cells[i].pos;
			if ((pos.x == selectedVoxel.x) && (pos.y == selectedVoxel.y)) {
				if (highestPoint.z < pos.z)
					highestPoint.z = pos.z;
			}
		}
		//Now we want to be 1 above the highest point
		selectedVoxel = highestPoint + vec3(0,0,1);
	}
		break;
	case MODE_DELETE: {
		pair<vec3,vec3> ray = camera->Unproject(cursorPosition);
		vec3 voxelHit;
		vec3 voxelNormal;
		//Mode delete is special with its own valid criteria
		if (beingEdited->EditorTraceToVoxel(ray.first,ray.second,voxelHit,voxelNormal)) {
			selectionValid = true;
			selectedVoxel = voxelHit;
		}
		else
			selectionValid = false;
		return;
	}
	
		break;
	}
	//Now limit the selectedVoxel to the permitted region
	selectionValid = ((beingEdited->Extents.x > selectedVoxel.x) && (beingEdited->Extents.y > selectedVoxel.y) && (beingEdited->Extents.z > selectedVoxel.z) &&
		(0 <= selectedVoxel.x) && (0 <= selectedVoxel.y) && (0 <= selectedVoxel.z));
}

//Specify the structure to edit
//and the size of one side of the square (starts at 0,0 and goes to size,size)
void Editor::EditStructure(Structure * structure) {
	beingEdited = structure;
	drawFloor.Resize(vec3(),structure->Extents);
}

//Read input not consumed by the dialog system
void Editor::ReadInput(vector<InputEvent> input) {
	for (auto e : input) {
		//Process mouse movements
		if (e.Event == InputEvent::MouseMove)
			UpdateCurrentVoxel(vec2(e.MouseX,e.MouseY));
		if (e.Event == InputEvent::KeyboardUp) {
			//Cycle modes using q and e
			if (e.Key == 'q')
				mode = (EditorMode)(((int)mode+1) % (int)MODE_LAST);
			else if (e.Key == 'e')
				mode = (EditorMode)(((int)mode == 0) ? ((int)MODE_LAST - 1) : ((int)mode - 1));
		}

	}
}

void Editor::SetMaterial(int materialId) {
	this->materialId = materialId;
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

//The user has requested the editor place a voxel at the current position
void Editor::PlaceVoxel() {
	if (selectionValid) {
		if (mode == MODE_DELETE)
			beingEdited->EditorRemoveVoxel(selectedVoxel);
		else
			beingEdited->EditorAddVoxel(selectedVoxel,materialId);
	}
		
}

//Draw the editor guidelines  
void Editor::Draw(GL3DProgram * shader) {
	_ASSERTE(beingEdited);
	//Draw the structure
	glDisable(GL_CULL_FACE);
	CurrentSystem->Textures.GetTexture("terrain/tiles-lowres.png")->Bind();
	beingEdited->EditorRenderStructure(shader,drawSystem);
	//Draw transparent objects
	//Use culling so sorting isn't necessary 
	glEnable(GL_CULL_FACE);
	if (selectionValid && (mode != MODE_DELETE))
		drawVoxel.Texture = "Interface/drawVoxel.png";
	else
		drawVoxel.Texture = "Interface/drawError.png";
	drawVoxel.Draw(shader,selectedVoxel-vec3(.05,.05,.05));
	drawFloor.Draw(shader,vec3());
}