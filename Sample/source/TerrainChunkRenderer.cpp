#include "stdafx.h"
#include "TerrainChunkRenderer.h"
#include "GLTerrainProgram.h"
#include "TerrainChunk.h"

TerrainChunkRenderer::TerrainChunkRenderer() {
	cout << "Chunk vertex data size: " << sizeof(TerrainChunk::ChunkVertexData) << "\n";
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
void TerrainChunkRenderer::HotChunk::Render(GLTerrainProgram * shader) {
	glBindVertexArray ( vertexArrayBuffer );
	/*glBindBuffer ( GL_ARRAY_BUFFER, vertexData );
	glVertexAttribPointer ( shader->AttributeVertex(), 3, GL_FLOAT, GL_FALSE, sizeof(TerrainChunk::ChunkVertexData), (void*)offsetof(TerrainChunk::ChunkVertexData,Vertex) );
	glVertexAttribPointer ( shader->AttributeTexture(), 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(TerrainChunk::ChunkVertexData), (void*)offsetof(TerrainChunk::ChunkVertexData,TextureCoordinateX) );
	glVertexAttribPointer ( shader->AttributeShading(), 1, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(TerrainChunk::ChunkVertexData), (void*)offsetof(TerrainChunk::ChunkVertexData,Shading) );*/
	glDrawArrays( GL_TRIANGLES, 0, (GLsizei) vertexCount);
    
    // Don't fuck the VAOs
    glBindVertexArray( 0 );
}
void TerrainChunkRenderer::HotChunk::Set(TerrainChunk * chunk, GLTerrainProgram * shader) {
	vertexCount = chunk->VertexDataSize;

	//Cache vertex data
	glBindVertexArray ( vertexArrayBuffer );
    
	//vertex positions
	glBindBuffer ( GL_ARRAY_BUFFER, vertexData );
	glBufferData ( GL_ARRAY_BUFFER, vertexCount*sizeof(TerrainChunk::ChunkVertexData), chunk->VertexData, GL_STATIC_DRAW );
	glEnableVertexAttribArray ( shader->AttributeVertex() );
	glVertexAttribPointer ( shader->AttributeVertex(), 3, GL_FLOAT, GL_FALSE, sizeof(TerrainChunk::ChunkVertexData), (void*)offsetof(TerrainChunk::ChunkVertexData,Vertex) );
	glEnableVertexAttribArray ( shader->AttributeTexture() );
	glVertexAttribPointer ( shader->AttributeTexture(), 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(TerrainChunk::ChunkVertexData), (void*)offsetof(TerrainChunk::ChunkVertexData,TextureCoordinateX) );
	glEnableVertexAttribArray( shader->AttributeShading() );
	glVertexAttribPointer ( shader->AttributeShading(), 1, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(TerrainChunk::ChunkVertexData), (void*)offsetof(TerrainChunk::ChunkVertexData,Shading) );
    
    // Don't fuck the VAOs
    glBindVertexArray( 0 );
}

void TerrainChunkRenderer::StartRendering(GLTerrainProgram * shader) {

}

void TerrainChunkRenderer::RenderTerrainChunk(vec2 tilePos, TerrainChunk * chunk, GLTerrainProgram * shader) {
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
		//The chunk has been uploaded and is no longer dirty
		chunk->Dirty = false;

		//Save the slot as in use
		renderSlots[chunkPosition] = openSlot;
		renderer = openSlot;
	}
	else if (chunk->Dirty) {
		//Re-set the renderer
		renderer = slotIterator->second;
		renderer->Set(chunk,shader);
		chunk->Dirty = false;
	}
	else
		renderer = slotIterator->second;

	//Run the renderer
	renderer->Render(shader);
	//Mark it as used
	renderer->Rendered = true;

	shader->Model.PopMatrix();
}


void TerrainChunkRenderer::FinishRendering(GLTerrainProgram * shader) {
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