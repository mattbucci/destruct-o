#include "stdafx.h"
#include "Structure.h"
#include "Tracer.h"
#include "VoxelDrawSystem.h"
#include "GL3DProgram.h"

//Attempt to ray trace to this structure
bool Structure::Trace(const vec3 & from, const vec3 & direction, vec3 & rayCollision, vec3 & surfaceNormal) {
	bool collision = false;
	float minDist = 10000;
	for (int i = 0; i != Cells.size(); i++) {
		vec3 colCandidate;
		vec3 candidateNormal;
		//Trace to each voxel and look for collisions with the ray
		if (Tracer::TraceToVoxel(from,direction,Cells[i].pos,colCandidate,candidateNormal)) {
			float dist = glm::length(from-colCandidate);
			if (dist < minDist) {
				minDist = dist;
				collision = true;
				rayCollision = colCandidate;
				surfaceNormal = candidateNormal;
			}
		}
	}
	return collision;
}

//Ray trace to a specific voxel position instead of intersection
bool Structure::EditorTraceToVoxel(const vec3 & from, const vec3 & direction, vec3 & voxelCollidedWith, vec3 & surfaceNormal) {
	bool collision = false;
	float minDist = 10000;
	for (int i = 0; i != Cells.size(); i++) {
		vec3 candidateNormal;
		vec3 collisionPos;
		//Trace to each voxel and look for collisions with the rwy
		if (Tracer::TraceToVoxel(from,direction,Cells[i].pos,collisionPos,candidateNormal)) {
			float dist = glm::length(from-collisionPos);
			if (dist < minDist) {
				minDist = dist;
				collision = true;
				voxelCollidedWith = Cells[i].pos;
				surfaceNormal = candidateNormal;
				_ASSERTE(candidateNormal.z < 1.5);
			}
		}
	}
	return collision;
}

//Remove voxel at position
void Structure::EditorRemoveVoxel(vec3 position) {
	for (auto it = Cells.begin();it != Cells.end();it++) {
		if (it->pos == position) {
			Cells.erase(it);
			return;
		}
	}
}

//Add voxel at position. If a voxel already exists in that position do nothing
void Structure::EditorAddVoxel(vec3 position, int materialId) {
	for (auto cell : Cells) {
		if (cell.pos == position) 
			return;
	}
	StructureCell cell;
	cell.pos = position;
	cell.material = materialId;
	Cells.push_back(cell);
}


//Render all voxels in this structure in a single pass
void Structure::EditorRenderStructure(GL3DProgram * shader, VoxelDrawSystem * drawSystem) {
	//Draw every voxel independantly like the terrain is drawn
	drawSystem->startDraw(shader);

	for (auto cell : Cells)
		drawSystem->pushVoxel(shader,cell.pos,cell.material);

	drawSystem->finishDraw(shader);
}


//Load a structure or floor out of disc
Structure * Structure::LoadStructure(string structureName) {
	//Load the file type appropriate for this platform
	//first determine the endian-ness of this platform
	unsigned char check[4] = {0,0,0,1};
	unsigned int * checkInt = (unsigned int*)check;
	bool platformIsLargeEndian = (*checkInt == 1);
	//Now load the file
	vector<unsigned char> fileData;

	if (platformIsLargeEndian)
		fileData = ReadFile(structureName + ".be.struct");
	else
		fileData = ReadFile(structureName + ".le.struct");

	//Check that the file data loaded
	if (fileData.size() < 8)
		return NULL;

	string magic = string((char*)&fileData[0],4);
	if (magic != "STRT") {
		cout << "magic number in structure file: \"" << structureName << "\" is incorrect\n";
		return NULL;
	}
	int filePos = 4;
	//Read the version number and then load accordingly
	uint32_t version;
	Read(version,fileData,filePos);

	Structure * loading = new Structure();

	//Read the extents of the structure
	Read(loading->Extents.x,fileData,filePos);
	Read(loading->Extents.y,fileData,filePos);
	Read(loading->Extents.z,fileData,filePos);
	//Read the size of the cell structure
	uint32_t cellCount;
	Read(cellCount,fileData,filePos);
	//Read all cells
	for (int i = 0; i < cellCount; i++) {
		StructureCell cell;
		Read(cell.material,fileData,filePos);
		Read(cell.pos.x,fileData,filePos);
		Read(cell.pos.y,fileData,filePos);
		Read(cell.pos.z,fileData,filePos);
		loading->Cells.push_back(cell);
	}

	return loading;
}

bool Structure::WriteFile(string name, vector<unsigned char> data) {
	SDL_RWops *file = SDL_RWFromFile(name.c_str(), "wb"); 
	if(!file) {
		cout << "Failed to write file \"" << name << "\"\n";
		return false;
	}
	SDL_RWwrite(file,(char*)&data[0],1,data.size());
	SDL_RWclose(file);

	return true;
}

vector<unsigned char> Structure::ReadFile(string name) {
	vector<unsigned char> data;
	SDL_RWops *file = SDL_RWFromFile(name.c_str(), "rb"); 
	long size;

	
	if(!file) {
		cout << "Failed to read file \"" << name << "\"\n";
		return vector<unsigned char>();
	}

	/*get filesize:*/
	SDL_RWseek(file , 0 , RW_SEEK_END);
	size = (long)SDL_RWtell(file);
	SDL_RWseek(file,0,RW_SEEK_SET);

	if (size <= 0)
		return vector<unsigned char>();

	/*read contents of the file into the vector*/
	data.resize(size);
	SDL_RWread(file,&data[0], 1, (size_t)size);

	SDL_RWclose(file);
	return data;
}

//Save a structure or floor to disc
void Structure::SaveStructure(string structureName) {
	//Generate a save file for small endian and large endian files at the same time
	//first determine the endian-ness of this platform
	unsigned char check[4] = {0,0,0,1};
	unsigned int * checkInt = (unsigned int*)check;
	bool platformIsLargeEndian = (*checkInt == 1);
	//Now make a copy of the file in platform endian format and swapped endian format
	vector<unsigned char> platformFile;
	vector<unsigned char> swappedFile;
	//Add a magic cookie to both
	char magic[] = "STRT";
	for (int i = 0; i < 4; i++) {
		platformFile.push_back(magic[i]);
		swappedFile.push_back(magic[i]);
	}
	//Write a version number
	Write((uint32_t)1,platformFile,swappedFile);
	//Now push structure data starting with extents
	Write(Extents.x,platformFile,swappedFile);
	Write(Extents.y,platformFile,swappedFile);
	Write(Extents.z,platformFile,swappedFile);
	//Write the size of the cell structure
	Write((uint32_t)Cells.size(),platformFile,swappedFile);
	//Write all cells
	for (auto cell : Cells) {
		Write(cell.material,platformFile,swappedFile);
		Write(cell.pos.x,platformFile,swappedFile);
		Write(cell.pos.y,platformFile,swappedFile);
		Write(cell.pos.z,platformFile,swappedFile);
	}
	//Now write the two files using SDL
	if (platformIsLargeEndian) {
		//Write big endian/little endian based off the platform
		WriteFile(structureName + ".be.struct",platformFile); 
		WriteFile(structureName + ".le.struct",swappedFile);
	}
	else {
		//Write big endian/little endian based off the platform
		WriteFile(structureName + ".le.struct",platformFile); 
		WriteFile(structureName + ".be.struct",swappedFile);
	}
}