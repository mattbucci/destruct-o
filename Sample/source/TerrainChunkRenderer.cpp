#include "stdafx.h"
#include "TerrainChunkRenderer.h"
#include "GL3DProgram.h"
#include "TerrainChunk.h"

TerrainChunkRenderer::TerrainChunkRenderer() {
	
}
TerrainChunkRenderer::~TerrainChunkRenderer() {
	//Destroy all the render slots
	for (auto slot : renderSlots) 
		delete slot.second;
	for (auto slot : openSlots)
		delete slot;
}
	
TerrainChunkRenderer::HotChunk::HotChunk() {
	glGenBuffers(1,&vertexData);
	glGenVertexArrays(1,&vertexArrayBuffer);
}
TerrainChunkRenderer::HotChunk::~HotChunk() {
	glDeleteBuffers(1,&vertexData);
	glDeleteVertexArrays(1,&vertexArrayBuffer);
}
//Render the set chunk
void TerrainChunkRenderer::HotChunk::Render(GL3DProgram * shader) {
	glBindVertexArray ( vertexArrayBuffer );
	glBindBuffer ( GL_ARRAY_BUFFER, vertexData );
	glVertexAttribPointer ( shader->AttributeVertex(), 3, GL_FLOAT, GL_FALSE, sizeof(TerrainChunk::ChunkVertexData), (void*)offsetof(TerrainChunk::ChunkVertexData,Vertex) );
	glVertexAttribPointer ( shader->AttributeTexture(), 1, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(TerrainChunk::ChunkVertexData), (void*)offsetof(TerrainChunk::ChunkVertexData,TextureCoordinate) );
	glDrawArrays( GL_TRIANGLES, 0, vertexCount);
}
void TerrainChunkRenderer::HotChunk::Set(TerrainChunk * chunk, GL3DProgram * shader) {
	vertexCount = chunk->VertexDataSize;

	glBindVertexArray ( vertexArrayBuffer );
	//vertex positions
	glBindBuffer ( GL_ARRAY_BUFFER, vertexData );
	glBufferData ( GL_ARRAY_BUFFER, vertexCount*sizeof(TerrainChunk::ChunkVertexData), chunk->VertexData, GL_DYNAMIC_DRAW );
	glEnableVertexAttribArray ( shader->AttributeVertex() );
	glVertexAttribPointer ( shader->AttributeVertex(), 3, GL_FLOAT, GL_FALSE, sizeof(TerrainChunk::ChunkVertexData), (void*)offsetof(TerrainChunk::ChunkVertexData,Vertex) );
	glEnableVertexAttribArray ( shader->AttributeTexture() );
	glVertexAttribPointer ( shader->AttributeTexture(), 1, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(TerrainChunk::ChunkVertexData), (void*)offsetof(TerrainChunk::ChunkVertexData,TextureCoordinate) );

	//glVertexAttribPointer ( shader->AttributeTexture(), 1, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(TerrainChunk::ChunkVertexData), (void*)offsetof(TerrainChunk::ChunkVertexData,TextureCoordinate) );
	
}

void TerrainChunkRenderer::StartRendering(GL3DProgram * shader) {

}

void TerrainChunkRenderer::RenderTerrainChunk(vec2 tilePos, TerrainChunk * chunk, GL3DProgram * shader) {
	//Draw the chunk relative to itself
	shader->Model.PushMatrix();
	shader->Model.Translate(chunk->X,chunk->Y,0);
	shader->Model.Apply();

	//Check if this chunk is already buffered
	vec2 chunkPosition = tilePos+vec2(chunk->X,chunk->Y);
	auto slotIterator = renderSlots.find(chunkPosition);
	HotChunk * renderer;
	if (slotIterator == renderSlots.end()) {
		//Doesn't exist, buffer it
		//find an open slot
		HotChunk * openSlot;
		if (openSlots.size() > 0) {
			openSlot = openSlots.back();
			openSlots.pop_back();
		}
		else
			openSlot = new HotChunk();

		//Set the slot to hold the given chunk
		openSlot->Set(chunk,shader);

		//Save the slot as in use
		renderSlots[chunkPosition] = openSlot;
		renderer = openSlot;
	}
	else
		renderer = slotIterator->second;

	//Run the renderer
	renderer->Render(shader);
	//Mark it as used
	renderer->Rendered = true;

	shader->Model.PopMatrix();
}


void TerrainChunkRenderer::FinishRendering(GL3DProgram * shader) {
	//Find renderers which were not used and mark them as free
	for (auto it = renderSlots.begin(); it != renderSlots.end();) {
		if (it->second->Rendered) {
			it->second->Rendered = false;
			it++;
		}
		else {
			openSlots.push_back(it->second);
			it = renderSlots.erase(it);
		}
	}
}