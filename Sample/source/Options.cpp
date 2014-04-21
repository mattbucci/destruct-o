#include "stdafx.h"
#include "Options.h"


Options::Options(void)
{
}


Options::~Options(void)
{
}


void Options::Print() {
	cout << "Options:" << endl;
	cout << "\tView Distance: " << ViewDistance << endl;
	cout << "\tHUD Transparency: " << HUDTransparency << endl;
}