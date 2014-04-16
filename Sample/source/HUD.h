
#pragma once

#include "stdafx.h"
#include "TexturedRect.h"

class GL2DProgram;

class HUD {
	ContiguousList<pair<double, vec2>> damagePoints;

	TexturedRect damageIndicator;
	TexturedRect minimapDot;
	TexturedRect minimapBackground;
public:
	HUD();
	//Since the hud is purely visual
	//there is no point in updating it at 100hz
	//it might as well update as its drawn
	void DrawAndUpdate(GL2DProgram * shader, vec2 viewPortSize);
};