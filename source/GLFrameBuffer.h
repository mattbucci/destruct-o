
#pragma once

class GLFrameBuffer {
	GLuint frameBuffer;
	GLuint * frameTextures;
	GLuint depthTexture;
	int width;
	int height;
	int frameTextureCount;
public:
	GLFrameBuffer(int width, int height, int bufferCount,bool enableDepth = true);

	//Allows each frame buffer to be written to
	void BindForWriting();

	//Bind each frame buffer as a texture consecutively
	//Also resets the frame buffer for write to the screen
	void BindForReading(int startingTextureId = 0);

	virtual ~GLFrameBuffer();
};