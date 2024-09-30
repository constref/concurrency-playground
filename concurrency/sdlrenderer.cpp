#include <SDL.h>
#include <sstream>
#include <vector>
#include <thread>

#include "sdlrenderer.h"

void SDLRenderer::drawPixel(int x, int y, uint32_t color)
{
	const int halfWidth = width / 2;
	const int halfHeight = height / 2;

	float ox = static_cast<float>(halfWidth + x);
	float oy = static_cast<float>(halfHeight + y);
	int offset = static_cast<int>(pitch * oy + ox * sizeof(uint32_t));

	uint32_t *intPtr = (uint32_t *)&pixels[offset];
	*intPtr = color;
}

void SDLRenderer::initialize(SDL_Window *window, int width, int height)
{
	this->width = width;
	this->height = height;

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	surface = SDL_GetWindowSurface(window);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);

	TTF_Init();
	largeFont = TTF_OpenFont("upheavtt.ttf", 72);
	smallFont = TTF_OpenFont("upheavtt.ttf", 48);
}

void SDLRenderer::shutdown()
{
	TTF_CloseFont(largeFont);
	TTF_CloseFont(smallFont);
	TTF_Quit();

	// clean up and shut down SDL
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}

void SDLRenderer::prepareFrame()
{
	SDL_RenderClear(renderer);
}

void SDLRenderer::displayFrame()
{
	// unlock the texture and display it
	SDL_RenderPresent(renderer);
}

void SDLRenderer::lockTexture()
{
	SDL_LockTexture(texture, nullptr, (void **)&pixels, &pitch);
}

void SDLRenderer::unlockTexture()
{
	SDL_UnlockTexture(texture);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
}

void SDLRenderer::drawBox(SDL_Rect &rect, const SDL_Color &color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, &rect);
}

void SDLRenderer::drawText(SDL_Rect &rect, SDL_Texture *texture)
{
	SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

TextLabel SDLRenderer::createLargeText(const std::string &text, const SDL_Color &color)
{
	SDL_Surface *surface = TTF_RenderText_Solid(largeFont, text.c_str(), color);
	TextLabel textLabel;
	textLabel.texture = SDL_CreateTextureFromSurface(renderer, surface);
	textLabel.rect.x = 0;
	textLabel.rect.y = 0;
	textLabel.rect.w = surface->w;
	textLabel.rect.h = surface->h;

	SDL_FreeSurface(surface);
	return textLabel;
}

TextLabel SDLRenderer::createSmallText(const std::string &text, const SDL_Color &color)
{
	SDL_Surface *surface = TTF_RenderText_Solid(smallFont, text.c_str(), color);
	TextLabel textLabel;
	textLabel.texture = SDL_CreateTextureFromSurface(renderer, surface);
	textLabel.rect.x = 0;
	textLabel.rect.y = 0;
	textLabel.rect.w = surface->w;
	textLabel.rect.h = surface->h;

	SDL_FreeSurface(surface);
	return textLabel;
}
