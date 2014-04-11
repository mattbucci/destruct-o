
#pragma once

#include "stdafx.h"

class TerrainChunk;
class GLTerrainProgram;

class TerrainChunkRenderer {
	

	//A render slot for chunks
	class HotChunk {
		TerrainChunk * toRender;
		GLuint vertexArrayBuffer;
		GLuint vertexData;
		int vertexCount;
	public:
		HotChunk();
		~HotChunk();

		//Set the chunk this hotchunk is supposed to render
		void Set(TerrainChunk * chunk, GLTerrainProgram * shader);
		//Render the set chunk
		void Render(GLTerrainProgram * shader);

		//Whether or not this chunk was rendered
		bool Rendered;
	};
	//A comparable 2d position
	struct ComparablePosition {
		ComparablePosition() {
			X = Y = 0;
		}
		ComparablePosition(vec2 from) {
			X = from.x;
			Y = from.y;
		}
		//Comparison operators necessary for std::map
		bool operator <(const ComparablePosition & other) const {
			if (X < other.X)
				return true;
			if (X > other.X)
				return false;
			return Y < other.Y;
		}
		bool operator ==(const ComparablePosition & other) const {
			return (X == other.X) && (Y == other.Y);
		}
		bool operator !=(const ComparablePosition & other) const {
			return (X != other.X) || (Y != other.Y);
		}

		float X;
		float Y;
	};

	//A list of all possible render slots which are in use
	map<ComparablePosition,HotChunk*> renderSlots;
	vector<HotChunk*> openSlots;
public:
	TerrainChunkRenderer();
	~TerrainChunkRenderer();
	
	void StartRendering(GLTerrainProgram * shader);

	//Must pass tile position, but the model matrix should already have set 0,0,0 to the tile's 0,0,0
	void RenderTerrainChunk(vec2 tilePos, TerrainChunk * chunk, GLTerrainProgram * shader);

	void FinishRendering(GLTerrainProgram * shader);
};