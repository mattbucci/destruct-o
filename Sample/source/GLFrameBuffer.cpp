#include "stdafx.h"
#include "GLFrameBuffer.h"

GLFrameBuffer::GLFrameBuffer(int width, int height, int bufferCount,bool enableDepth) {
	//Max of 8 buffers right now
	_ASSERTE(bufferCount <= 8);

	depthTexture = 0;
	frameTextures = new GLuint[bufferCount];
	//Create the FBO
	glGenFramebuffers(1, &frameBuffer); 
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);

	//Create the gbuffer textures
	glGenTextures(bufferCount, frameTextures);
	
	//Build a texture for each color target in the frame buffer
	for (int i = 0; i < bufferCount; i++) {
		glBindTexture(GL_TEXTURE_2D, frameTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, frameTextures[i], 0);
	}

	//depth
	if (enableDepth) {
		glGenTextures(1, &depthTexture);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	}


	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
							 GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7}; 
	glDrawBuffers(bufferCount, DrawBuffers);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "Failed to setup all frame buffers. May not be compatible with this GPU.";

	//restore default FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	//Save constructor information for future use
	this->width = width;
	this->height = height;
	frameTextureCount = bufferCount;
}

//Allows each frame buffer to be written to
void GLFrameBuffer::BindForWriting() {
	// Render to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glViewport(0,0,width,height);
}

//Bind each frame buffer as a texture consecutively
//Also resets the frame buffer for write to the screen
void GLFrameBuffer::BindForReading(int startingTextureId) {
	//restore default FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glEnable(GL_TEXTURE_2D);
	//Activate each buffer as a texture starting at startingTextureId
	for (int i = 0; i < frameTextureCount; i++) {
		glActiveTexture (GL_TEXTURE0+i+startingTextureId);
		//Bind texture
		glBindTexture(GL_TEXTURE_2D,frameTextures[i]);
	}

}

GLFrameBuffer::~GLFrameBuffer() {
	glDeleteTextures(frameTextureCount,frameTextures);
	glDeleteFramebuffers(1,&frameBuffer);
}