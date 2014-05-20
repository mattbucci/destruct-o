#include "stdafx.h"
#include "TerrainChunk.h"
#include "GameTile.h"

struct PODVec3Comparator
{
    bool operator() (const Utilities::PODVec3 a, const Utilities::PODVec3 b) const
    {
        float aL = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
        float bL = sqrt(b.x*b.x + b.y*b.y + b.z*b.z);
        return aL < bL;
    }
};

static bool operator== (const Utilities::PODVec3& a, const Utilities::PODVec3& b)
{
    return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}

TerrainChunk::TerrainChunk(GameTile * owningTile, int chunkx, int chunky) {
	this->owner = owningTile;
	X = (float)chunkx;
	Y = (float)chunky;
	VertexData = NULL;
    VertexIndices = NULL;
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
	quad(vec3 a1, vec3 b1, vec3 a2, vec3 b2, int direction) {
		this->a1 = INV(a1);
		this->b1 = INV(b2);
		this->a2 = INV(b1);
		this->b2 = INV(a2);

		ASize = abs(glm::dot(vec3(1,1,1),this->a2-this->a1));
		BSize = abs(glm::dot(vec3(1,1,1),this->b1-this->a1));
		this->direction = direction;
	}
	//distance between a1 and a2
	float ASize;
	//distance between a1 and b1
	float BSize;

	//The major axis the normal of this quad exists in
	int direction;

	vec3 a1;
	vec3 b1;
	vec3 a2;
	vec3 b2;
};

//Check if the given ijk contains a voxel
bool TerrainChunk::f(int i, int j, int k) {
	_ASSERTE((i < CHUNK_SIZE) && (k < CHUNK_SIZE));

	i += (int)Y;
	k += (int)X;


	//ijk is yzx
	TileCell & cell = owner->Cells[i*TILE_SIZE+k];

	return ((j <= cell.height) && (j >= cell.height-cell.stackHeight));
}
//check if the given position is beneath the surface
bool TerrainChunk::fu(int i, int j, int k) {
	if ((i >= CHUNK_SIZE) || (i < 0) || (k >= CHUNK_SIZE) || (k < 0) || (j >= 256) || (j < 0))
		return false;

	i += (int)Y;
	k += (int)X;


	//ijk is yzx
	TileCell & cell = owner->Cells[i*TILE_SIZE+k];

	return (j < cell.height-cell.stackHeight);
}


//Reconstruct the vertex array using the owned game tile
void TerrainChunk::Reconstruct() {
	//Mark this tile as dirty
	Dirty = true;
	int minHeight = 256;
	int maxHeight = 0;
	//Calc chunk limits
	//and count voxels
	int ceX = (int)X + CHUNK_SIZE;
	int ceY = (int)Y + CHUNK_SIZE;
	VoxelCount= 0;
	for (int y = (int)Y; y < ceY; y++) {
		for (int x = (int)X; x < ceX; x++) {
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

	//Mask now uses stack
	int mask[CHUNK_SIZE*256];

	//Sweep over 3-axes
	vector<quad> quads;
	for(int d=0; d<3; ++d) {
		int i, j, k, l, w, h
			, u = (d+1)%3
			, v = (d+2)%3;
		float x[3] = {0,0,0};
		float q[3] = {0,0,0};
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
					if (d == 1) {
						mask[n-1] =
							((0    <= x[d]      ? f(x[0],      x[1],      x[2])      : false)) &&
							(!(x[d] <  dims[d]-1 ? f(x[0]+q[0], x[1]+q[1], x[2]+q[2]) : false));
					}
					else {
						mask[n-1] = mask[n-1] && !( fu(x[0]+q[0], x[1]+q[1], x[2]+q[2]) ||  fu(x[0], x[1], x[2]) );
					}
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
							,d));
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
	}

    // Four verts per quad
	VertexDataSize = quads.size() * 4;
    VertexIndicesSize = quads.size() * 6;
    
	delete [] VertexData;
	VertexData = new ChunkVertexData[VertexDataSize];
    VertexIndices = new GLushort[VertexIndicesSize];
	int vcount = 0, icount = 0;
    typedef std::map<Utilities::PODVec3, int, PODVec3Comparator> chunkmap;
    chunkmap chunks;
    chunkmap::iterator it;
    ChunkVertexData vertex;
    
	for (unsigned int i = 0; i < quads.size(); i++) {
		unsigned char terX, terY;
		int materialVotes[16];
		memset(materialVotes,0,sizeof(int)*16);
		//Test 5 random points and use the one with the most votes
		vec3 quadMin = vec3(100000,100000,10000);
		quadMin = glm::min(quadMin,quads[i].a1);
		quadMin = glm::min(quadMin,quads[i].a2);
		quadMin = glm::min(quadMin,quads[i].b1);
		quadMin = glm::min(quadMin,quads[i].b2);
		vec3 quadMax = vec3(-10000,-10000,-10000);
		quadMax = glm::max(quadMax,quads[i].a1);
		quadMax = glm::max(quadMax,quads[i].a2);
		quadMax = glm::max(quadMax,quads[i].b1);
		quadMax = glm::max(quadMax,quads[i].b2);

		int materialWithMostVotes = 0;
		int mostVotes = 0;

		for (int m = 0; m < 5; m++) {
			vec2 test = vec2(Utilities::random(quadMin.x,quadMax.x),Utilities::random(quadMin.y,quadMax.y)) + vec2(X,Y);
			test = glm::floor(test);
			test = glm::max(test,vec2());
			test = glm::min(test,vec2(TILE_SIZE-1,TILE_SIZE-1));
			TileCell & cell = owner->Cells[(int)test.y*TILE_SIZE+(int)test.x];
			_ASSERTE(cell.materialId < 16);
			materialVotes[cell.materialId]++;
			if (materialVotes[cell.materialId] > mostVotes) {
				mostVotes = materialVotes[cell.materialId];
				materialWithMostVotes = cell.materialId;
			}
		}

		        
        // Generate interleaved data for each vertex of each found quad
        GLushort a1 = vcount;
		vertex.TextureCoordinateX = 0;
		vertex.TextureCoordinateY = 0;
		vertex.Material = materialWithMostVotes;
		vertex.Vertex = toPOD(quads[i].a1);
        VertexData[vcount++] = vertex;
        
        GLushort a2 = vcount;
		vertex.TextureCoordinateX = 0;
		vertex.TextureCoordinateY = (uint8_t)quads[i].BSize;
		vertex.Material = materialWithMostVotes;
		vertex.Vertex = toPOD(quads[i].b1);
        VertexData[vcount++] = vertex;
        
        GLushort b1 = vcount;
		vertex.TextureCoordinateX = (uint8_t)quads[i].ASize;
		vertex.TextureCoordinateY = 0;
		vertex.Material = materialWithMostVotes;
		vertex.Vertex = toPOD(quads[i].a2);
        VertexData[vcount++] = vertex;
        
        GLushort b2 = vcount;
		vertex.TextureCoordinateX = (uint8_t)quads[i].ASize;
		vertex.TextureCoordinateY = (uint8_t)quads[i].BSize;
		vertex.Material = materialWithMostVotes;
		vertex.Vertex = toPOD(quads[i].b2);
        VertexData[vcount++] = vertex;
        
        // Generate the indices (first triangle)
        VertexIndices[icount++] = a1;
        VertexIndices[icount++] = a2;
        VertexIndices[icount++] = b1;

        // second triangle
        VertexIndices[icount++] = a2;
        VertexIndices[icount++] = b1;
        VertexIndices[icount++] = b2;
	}
}
