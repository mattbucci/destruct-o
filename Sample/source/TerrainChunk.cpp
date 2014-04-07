#include "stdafx.h"
#include "TerrainChunk.h"
#include "GameTile.h"

TerrainChunk::TerrainChunk(GameTile * owningTile, int chunkx, int chunky) {
	this->owner = owningTile;
	X = (float)chunkx;
	Y = (float)chunky;
	VertexData = NULL;
	Reconstruct();
}
TerrainChunk::~TerrainChunk() {
	delete [] VertexData;
}

//This corrects the order of xyz
#define INV(a) vec3(a.z,a.x,a.y)

struct quad {
	quad() {
		
	}
	quad(vec3 a1, vec3 b1, vec3 a2, vec3 b2) {
		this->a1 = INV(a1);
		this->b1 = INV(b2);
		this->a2 = INV(b1);
		this->b2 = INV(a2);
	}
	vec3 a1;
	vec3 b1;
	vec3 a2;
	vec3 b2;
};

bool TerrainChunk::f(int i, int j, int k) {
	_ASSERTE((i < CHUNK_SIZE) && (k < CHUNK_SIZE));

	i += (int)Y;
	k += (int)X;


	//ijk is yzx
	TileCell & cell = owner->Cells[i*TILE_SIZE+k];

	return ((j <= cell.height) && (j >= cell.height-cell.stackHeight));
}


//Reconstruct the vertex array using the owned game tile
void TerrainChunk::Reconstruct() {
	int minHeight = 256;
	int maxHeight = 0;
	//Calc chunk limits
	//and count voxels
	int ceX = X + CHUNK_SIZE;
	int ceY = Y + CHUNK_SIZE;
	VoxelCount= 0;
	for (int y = Y; y < ceY; y++) {
		for (int x = X; x < ceX; x++) {
			TileCell & cell = owner->Cells[y*TILE_SIZE+x];
			VoxelCount+= cell.stackHeight+1;
			minHeight = min((int)cell.height,minHeight);
			maxHeight = max((int)cell.height,maxHeight);
		}
	}

	//Find the z extents
	int mdims[3] = {
		-1,
		max(-1,minHeight-2),
		-1,
	};
	int dims[3] = {
		CHUNK_SIZE,
		min(256,maxHeight+2),
		CHUNK_SIZE,
	};
	
	


	//START OF MODIFIED GREEDY-MESH ALGORITHM
	//ORIGINAL: http://0fps.net/2012/06/30/meshing-in-a-minecraft-game/
	//ORIGINAL IMPLEMENTATION IN JAVASCRIPT: https://github.com/mikolalysenko/mikolalysenko.github.com/blob/gh-pages/MinecraftMeshes/js/greedy.js
	//MODIFIED FOR DESTRUCTO (no interior edges)

	//Sweep over 3-axes
	vector<quad> quads;
	for(int d=0; d<3; ++d) {
		int i, j, k, l, w, h
			, u = (d+1)%3
			, v = (d+2)%3;
		float x[3] = {0,0,0};
		float q[3] = {0,0,0};
		int *mask = new int[dims[u] * dims[v]];
		q[d] = 1;
		for(x[d]=mdims[d]; x[d]<dims[d]; ) {
			//Compute mask
			int n = 0;
			for(x[v]=0; x[v]<dims[v]; ++x[v])
				for(x[u]=0; x[u]<dims[u]; ++x[u]) {
					mask[n++] =
						(0    <= x[d]      ? f(x[0],      x[1],      x[2])      : false) !=
						(x[d] <  dims[d]-1 ? f(x[0]+q[0], x[1]+q[1], x[2]+q[2]) : false);
					//Inject code here to remove interior edges
					/*if (d == 1) {
						mask[n-1] =
							(!(0    <= x[d]      ? f(x[0],      x[1],      x[2])      : false)) &&
							(x[d] <  dims[d]-1 ? f(x[0]+q[0], x[1]+q[1], x[2]+q[2]) : false);
					}*/
				}
				//Increment x[d]
				++x[d];
				//Generate mesh for mask using lexicographic ordering
				n = 0;
				for(j=0; j<dims[v]; ++j)
					for(i=0; i<dims[u]; ) {
						if(mask[n]) {
							//Compute width
							for(w=1; mask[n+w] && i+w<dims[u]; ++w) {
							}
							//Compute height (this is slightly awkward
							bool done = false;
							for(h=1; j+h<dims[v]; ++h) {
								for(k=0; k<w; ++k) {
									if(!mask[n+k+h*dims[u]]) {
										done = true;
										break;
									}
								}
								if(done) {
									break;
								}
							}
							//Add quad
							x[u] = i;  x[v] = j;
							float du[3] = {0,0,0}; du[u] = w;
							float dv[3] = {0,0,0}; dv[v] = h;
							quads.push_back(quad(
								vec3(x[0],             x[1],             x[2]            )
								, vec3(x[0]+du[0],       x[1]+du[1],       x[2]+du[2]      )
								, vec3(x[0]+du[0]+dv[0], x[1]+du[1]+dv[1], x[2]+du[2]+dv[2])
								, vec3(x[0]      +dv[0], x[1]      +dv[1], x[2]      +dv[2])
							));
							//Zero-out mask
							for(l=0; l<h; ++l)
								for(k=0; k<w; ++k) {
									mask[n+k+l*dims[u]] = false;
								}
								//Increment counters and continue
								i += w; n += w;
						} else {
							++i;    ++n;
						}
					}
		}
		//Cleanup mask
		delete [] mask;
	}

	//cout << "[" << (unsigned int)this << "] simplified into: " << quads.size() << " quads or " << quads.size()*6 << " vertices.\n";

	VertexDataSize = quads.size()*6; 
	delete [] VertexData;
	VertexData = new ChunkVertexData[VertexDataSize];
	int vcount = 0;
	for (int i = 0; i < quads.size(); i++) {
		//Generate interleaved data for each vertex
		//of each found quad
		//First triangle
		//VertexData[vcount].TextureCoordinate = 0;
		VertexData[vcount++].Vertex = quads[i].a1;
		//VertexData[vcount].TextureCoordinate = 2;
		VertexData[vcount++].Vertex = quads[i].b1;
		//VertexData[vcount].TextureCoordinate = 1;
		VertexData[vcount++].Vertex = quads[i].a2;

		//Second triangle
		//VertexData[vcount].TextureCoordinate = 2;
		VertexData[vcount++].Vertex = quads[i].b1;
		//VertexData[vcount].TextureCoordinate = 1;
		VertexData[vcount++].Vertex = quads[i].a2;
		//VertexData[vcount].TextureCoordinate = 3;
		VertexData[vcount++].Vertex = quads[i].b2;
	}
}