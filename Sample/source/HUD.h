
#pragma once

#include "stdafx.h"
#include "TexturedRect.h"

class BaseFrame;
class ActorPlayer;
class GL2DProgram;

class HUD {
	ContiguousList<pair<double, vec2>> damagePoints;

	BaseFrame* baseFrame;
	ActorPlayer* player;

	float minimapScale;

	TexturedRect damageIndicator;
	TexturedRect minimapDot;
	TexturedRect minimapBackground;

	HUD();
public:
	HUD(BaseFrame* baseFrame);
	//Since the hud is purely visual
	//there is no point in updating it at 100hz
	//it might as well update as its drawn
	void DrawAndUpdate(GL2DProgram * shader, vec2 viewPortSize);
};