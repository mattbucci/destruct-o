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
void Editor::Region::Resize(vec3 cornerA, vec3 cornerB, bool renderTop) {
	static vec3 sides[3][3] = {
		//First two are the coordinates that vary, the third is the coordinate that does not
		{vec3(1,0,0),vec3(0,1,0),vec3(0,0,1)},
		{vec3(1,0,0),vec3(0,0,1),vec3(0,1,0)},
		{vec3(0,1,0),vec3(0,0,1),vec3(1,0,0)},
	}; 
	int vertex = 0;

	vec3 extents = cornerB-cornerA;
	//Add two sides to the region per set of coordiantes. Very messy
	for (int i = 0; i < 3; i++) {
		vec3 pos;
		//First do one side
		pos = vec3();
		xat(vertex) = vec2();
		vat(vertex++) = cornerA*(1.0f-pos) + cornerB*pos;

		pos = (sides[i][0]);
		xat(vertex) = vec2(glm::dot(vec3(1,1,1),(sides[i][0]*extents)),0);
		vat(vertex++) = cornerA*(1.0f-pos) + cornerB*pos;

		pos = (sides[i][1]);
		xat(vertex) = vec2(0,glm::dot(vec3(1,1,1),(sides[i][1]*extents)));
		vat(vertex++) = cornerA*(1.0f-pos) + cornerB*pos;

		pos = (sides[i][0]);
		xat(vertex) = vec2(glm::dot(vec3(1,1,1),(sides[i][0]*extents)),0);
		vat(vertex++) = cornerA*(1.0f-pos) + cornerB*pos;

		pos = (sides[i][0]+sides[i][1]);
		xat(vertex) = vec2(glm::dot(vec3(1,1,1),(sides[i][0]*extents)),glm::dot(vec3(1,1,1),(sides[i][1]*extents)));
		vat(vertex++) = cornerA*(1.0f-pos) + cornerB*pos;

		pos = (sides[i][1]);
		xat(vertex) = vec2(0,glm::dot(vec3(1,1,1),(sides[i][1]*extents)));
		vat(vertex++) = cornerA*(1.0f-pos) + cornerB*pos;

		//Then the other side
		if ((i == 0) && (!renderTop))
			//Skip the top
			continue;
		pos = vec3();
		xat(vertex) = vec2();
		vat(vertex++) = cornerA*(1.0f-pos) + cornerB*pos+extents*sides[i][2];

		pos = (sides[i][0]);
		xat(vertex) = vec2(glm::dot(vec3(1,1,1),(sides[i][0]*extents)),0);
		vat(vertex++) = cornerA*(1.0f-pos) + cornerB*pos+extents*sides[i][2];

		pos = (sides[i][1]);
		xat(vertex) = vec2(0,glm::dot(vec3(1,1,1),(sides[i][1]*extents)));
		vat(vertex++) = cornerA*(1.0f-pos) + cornerB*pos+extents*sides[i][2];

		pos = (sides[i][0]);
		xat(vertex) = vec2(glm::dot(vec3(1,1,1),(sides[i][0]*extents)),0);
		vat(vertex++) = cornerA*(1.0f-pos) + cornerB*pos+extents*sides[i][2];

		pos = (sides[i][0]+sides[i][1]);
		xat(vertex) = vec2(glm::dot(vec3(1,1,1),(sides[i][0]*extents)),glm::dot(vec3(1,1,1),(sides[i][1]*extents)));
		vat(vertex++) = cornerA*(1.0f-pos) + cornerB*pos+extents*sides[i][2];

		pos = (sides[i][1]);
		xat(vertex) = vec2(0,glm::dot(vec3(1,1,1),(sides[i][1]*extents)));
		vat(vertex++) = cornerA*(1.0f-pos) + cornerB*pos+extents*sides[i][2];
	}
	//Adjust all the texture coordinates to work with the voxel renderer
	for (int i = 0; i < vertex; i++)
		xat(i) *= 4.0f;
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
	//Editor starts disabled
	enabled = false;
};

void Editor::EnableEditor(bool enable) {
	enabled = enable;
}

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
	drawFloor.Resize(vec3(),structure->Extents,false);
}

//Read input not consumed by the dialog system
void Editor::ReadInput(vector<InputEvent> input) {
	//If not enabled do not process input
	if (!enabled)
		return;

	for (auto e : input) {
		//Process mouse movements
		if (e.Event == InputEvent::MouseMove)
			UpdateCurrentVoxel(vec2(e.MouseX,e.MouseY));
		if (e.Event == InputEvent::KeyboardUp) {
			//Cycle modes using q and e
			if (e.Key == 'e')
				mode = (EditorMode)(((int)mode+1) % (int)MODE_LAST);
			else if (e.Key == 'q')
				mode = (EditorMode)(((int)mode == 0) ? ((int)MODE_LAST - 1) : ((int)mode - 1));
			else if (e.Key == 'r')
				currentLevel++;
			else if (e.Key == 'f')
				currentLevel--;
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
	//If not enabled draw nothing
	if (!enabled)
		return;

	//Draw the structure
	glDisable(GL_CULL_FACE);
	CurrentSystem->Textures.GetTexture("terrain/tiles-lowres.png")->Bind();
	beingEdited->EditorRenderStructure(shader,drawSystem);
	//Draw Regions

	//Flip the lights off to render things without shading
	shader->Lights.Off();
	if (selectionValid && (mode != MODE_DELETE))
		drawVoxel.Texture = "Interface/drawVoxel.png";
	else
		drawVoxel.Texture = "Interface/drawError.png";
	drawVoxel.Draw(shader,selectedVoxel-vec3(.05,.05,.05));
	drawFloor.Draw(shader,vec3());
	shader->Lights.On();
}