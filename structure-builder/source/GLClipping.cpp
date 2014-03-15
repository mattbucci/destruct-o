#include "stdafx.h"
#include "GLClipping.h"

GLClipping::GLClipping(GLuint clippingUniform) {
	this->clippingUniform = clippingUniform;
}

void GLClipping::PushClippingRect(const GLModel & modelMatrix, Rect region) {
	//Transform region to construct a new region
	vec3 upperLeft = vec3(region.X, region.Y,0);
	vec3 lowerRight = vec3(region.X+region.Width,region.Y+region.Height,0);
	upperLeft = modelMatrix.TransformVector(upperLeft);
	lowerRight = modelMatrix.TransformVector(lowerRight);
	
	if (clippingRects.size() > 0) {
		Rect oClippingRegion = clippingRects[clippingRects.size()-1];
		//Now subtract the requested clipping region from the current clipping region
		if (upperLeft.x < oClippingRegion.X)
			upperLeft.x = oClippingRegion.X;
		if (upperLeft.y < oClippingRegion.Y)
			upperLeft.y = oClippingRegion.Y;
		if (lowerRight.x > (oClippingRegion.X+oClippingRegion.Width))
			lowerRight.x = (oClippingRegion.X+oClippingRegion.Width);
		if (lowerRight.y > (oClippingRegion.Y+oClippingRegion.Height))
			lowerRight.y = (oClippingRegion.Y+oClippingRegion.Height);

		//Finally check that the upper left is more upper and left than lower right
		if (upperLeft.x > lowerRight.x)
			upperLeft.x = lowerRight.x;
		if (upperLeft.y > lowerRight.y)
			upperLeft.y = lowerRight.y;

	}
	//Push to the stack and apply the clipping region
	clippingRects.push_back(Rect(upperLeft.x,upperLeft.y,lowerRight.x-upperLeft.x,lowerRight.y-upperLeft.y));
	glUniform4f(clippingUniform,upperLeft.x,upperLeft.y,lowerRight.x,lowerRight.y);

}
void GLClipping::PopClippingRect() {
	_ASSERTE(clippingRects.size() > 0);

	clippingRects.pop_back();
	if (clippingRects.size() <= 0) {
		//disable clipping
		glUniform4f(clippingUniform,-1,-1,-1,-1);
	}
	else {
		//Apply the previous clipping region
		Rect oClippingRegion = clippingRects[clippingRects.size()-1];
		glUniform4f(clippingUniform,oClippingRegion.X,oClippingRegion.Y,oClippingRegion.X+oClippingRegion.Width,oClippingRegion.Y+oClippingRegion.Height);
	}
}