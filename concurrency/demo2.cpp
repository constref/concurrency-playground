#include <chrono>
#include <array>
#include <numbers>

#include "pixel.h"
#include "demo2.h"

using namespace std;

DemoRayCaster::DemoRayCaster() : SDLPlatform(800, 600, "Ray Casting")
{
	playerHeight = 32;
	boxSize = 64;

	playerPos = { .x = 0, .y = 0 };
	playerAngle = 0;
}

void DemoRayCaster::draw(double globalTime, float dt)
{
	const int rows = 16;
	const int cols = 16;

	std::array<int, rows * cols> world =
	{
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
		1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
		1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
		1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
		1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	};

	const float fov = 60;
	const double fovRads = 60 * (std::numbers::pi) / 180;
	const int projWidth = width;
	const int projHeight = height;
	const int halfWidth = width / 2;
	const int halfHeight = height / 2;
	const float projDist = halfWidth / tan(fovRads);
	const float rayDist = fov / 360;

	const int worldWidth = boxSize * cols;
	const int worldHeight = boxSize * rows;

	for (int x = -halfWidth; x <= halfWidth; ++x)
	{
		for (int y = -halfHeight; y <= halfHeight; ++y)
		{
			pixel p(255, y, y, 255);
			drawPixel(x, y, p.intValue);
		}
	}
	if (playerAngle <= std::numbers::pi)
	{
		// facing upwards

	}
	else
	{

	}
}

int main(int argc, char *argv[])
{
	DemoRayCaster demo;
	demo.execute();

	return 0;
}
