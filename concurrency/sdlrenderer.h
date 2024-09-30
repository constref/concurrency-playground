#pragma once

#include <SDL2/SDL_ttf.h>
#include "platform.h"
#include "pixel.h"

struct SDL_Renderer;
struct SDL_Window;
struct SDL_Surface;
struct SDL_Texture;

struct TextLabel
{
	SDL_Rect rect;
	SDL_Texture *texture;
};

class SDLRenderer
{
	std::string title;
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Surface *surface;
	SDL_Texture *texture;
	uint8_t *pixels;
	int pitch;
	unsigned long frameCount;
	int width, height;
	TTF_Font *largeFont, *smallFont;

public:
	void initialize(SDL_Window *window, int width, int height);
	void shutdown();

	void prepareFrame();
	void displayFrame();
	void lockTexture();
	void unlockTexture();
	void drawPixel(int x, int y, uint32_t color);
	void drawBox(SDL_Rect &rect, const SDL_Color &color);
	TextLabel createLargeText(const std::string &text, const SDL_Color &color);
	TextLabel createSmallText(const std::string &text, const SDL_Color &color);
	void drawText(SDL_Rect &rect, SDL_Texture *texture);
};