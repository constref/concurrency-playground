#pragma once

#include "sdlplatform.h"

struct vec2
{
	int x, y;
};

class DemoRayCaster : public SDLPlatform
{
	vec2 playerPos;
	float playerAngle;
	int playerHeight;
	int boxSize;

public:
	DemoRayCaster();

	void draw(double globalTime, float dt) override;
};