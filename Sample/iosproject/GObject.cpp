#include "stdafx.h"
#include "GObject.h"
#include <sstream>

//No 0 id
GID GObject::currentMaxId = 1;

GObject::GObject(void) {
	//Set your id to the next currentMaxId
	id = currentMaxId++;
}


GObject::~GObject(void) {
	//No way to reclaim used ids
}


	
GObject::GObject(const GObject & other) {
	_ASSERTE(false); //No copying game objects!
}
	
GObject & GObject::operator=(const GObject & other) {
	_ASSERTE(false); //No copying game objects!

	return *this;
}

GID GObject::GetId() {
	return id;
}

string GObject::GetTypeName() {
	stringstream basic;
	basic << "GObject#" << GetId() << "\n";
	return basic.str();
}