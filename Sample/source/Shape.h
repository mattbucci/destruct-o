
#pragma once

#include "Control.h"

//Though a LuaObject, is not exposed to lua at this level
class Shape : public Control {
	int vertexCount;
protected:

	virtual void recalculatePosition() override;
public:
	Shape(int vertexCount);
	~Shape();

	//Set verticies to a specific shape
	void SetToSquare(void);
	void SetToUpTriangle(void);
	void SetToDownTriangle(void);

	//Sets calculatedSize
	//Only useful if you plan on drawing this shape manually
	//If you use drawChildren() the calculated size will be regenerated
	void OverrideCalculatedSize(Rect newSize);

	virtual void Draw(GL2DProgram * shaders) override;

	void vset(int i, vec2 v);
};