#pragma once

#include <SDL.h>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <fstream>

struct SDL_Window;

template <class Renderer>
class Platform
{
protected:
	std::string title;
	SDL_Window *window;
	Renderer renderer;

	std::chrono::steady_clock::time_point prevTime, workStart, workEnd;
	double globalTime, dt;
	double titleUpdateTime;
	unsigned long frameCount;
	bool done;

	int width, height;
	bool useSingleThread;

public:
	Platform(int width, int height, const std::string &title) : width(width), height(height), title(title)
	{
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
		window = SDL_CreateWindow("Concurrency", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);

		done = false;
		useSingleThread = true;
		globalTime = 0;
		titleUpdateTime = 0;
		frameCount = 0;
		dt = 0;

		renderer.initialize(window, width, height);
		prevTime = std::chrono::high_resolution_clock::now();
	}

	virtual ~Platform()
	{
		renderer.shutdown();
		// clean up and shut down SDL
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	void startFrame()
	{
		// deal with time tracking and dt calculation
		auto nowTime = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<double, std::chrono::seconds::period>(nowTime - prevTime).count();
		prevTime = nowTime;
		globalTime += dt;
		titleUpdateTime += dt;

		// check platform events
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
			{
				done = true;
			}
			else if (event.type == SDL_KEYUP)
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_F1:
					{
						globalTime = 0;
						frameCount = 0;
						break;
					}
					case SDLK_F2:
					{
						globalTime = 0;
						frameCount = 0;
						useSingleThread = !useSingleThread;
						break;
					}
				}
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				mouseButton(event.button.x, event.button.y, true, event.button.button, event.button.clicks);
			}
			else if (event.type == SDL_MOUSEBUTTONUP)
			{
				mouseButton(event.button.x, event.button.y, false, event.button.button, event.button.clicks);
			}
		}

		workStart = std::chrono::high_resolution_clock::now(); // track start of work time
		renderer.prepareFrame();
	}

	void endFrame()
	{
		renderer.displayFrame();
		workEnd = std::chrono::high_resolution_clock::now(); // end of work time

		frameCount++;

		// window title updates happen every 1/2sec
		if (titleUpdateTime > 0.5f)
		{
			// calculate the time taken to compute pixels
			std::chrono::duration<double> workDuration(workEnd - workStart);
			const double secs(workDuration.count());

			// update the window titlebar
			std::ostringstream ss;
			ss << (useSingleThread ? " [Single] " : " [Multi] ") << title << " - Frame Time: " << secs
				<< " - FPS: " << frameCount / titleUpdateTime;
			frameCount = 0;

			SDL_SetWindowTitle(window, ss.str().c_str());
			titleUpdateTime -= 0.5f;
		}
	}

	bool intersection(int x, int y, const SDL_Rect &rect)
	{
		int minX = rect.x;
		int maxX = rect.x + rect.w;
		int minY = rect.y;
		int maxY = rect.y + rect.h;

		if (x > minX && x < maxX && y > minY && y < maxY)
		{
			return true;
		}
		return false;
	}

	virtual void mouseButton(int x, int y, bool isDown, int buttonIndex, int clicks) = 0;
};