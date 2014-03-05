#include "stdafx.h"
#include "GLAcidTab.h"


GLAcidTab::GLAcidTab(GLint timeUniform, GLint factorUniform) {
	this->timeUniform = timeUniform;
	this->factorUniform = factorUniform;
}
GLAcidTab::~GLAcidTab() {

}
	
//The current update cycle time would work nicely here
void GLAcidTab::SetCurrentTime(double time) {
	//In the future the value of time should wrap
	//so as to always keep its precision
	glUniform1f(timeUniform,(float)fmod(time,100));
}
//The amount the acid effects should be felt by the player
//0 means none
//1 means all
void GLAcidTab::SetAcidFactor(float factor) {
	glUniform1f(factorUniform,factor);
}