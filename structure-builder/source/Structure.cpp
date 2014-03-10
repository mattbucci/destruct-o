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