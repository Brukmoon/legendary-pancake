#include "graphics.h"

void put_pixel(SDL_Surface *surface,
	const int x, const int y,
	Uint8 r, Uint8 g, Uint8 b)
{
	if (surface == NULL || x >= surface->w || y >= surface->h || x < 0 || y < 0)
		return;
	// Some surfaces must be locked before they can be written to
	if (SDL_MUSTLOCK(surface))
		if (SDL_LockSurface(surface) != 0)
			return;
	Uint32 color, *target;
	// Map RGB values to a SDL_Color structure according to the surface's format
	color = SDL_MapRGB(surface->format, r, g, b);
	// Get pointer to the target pixel's color internal representation.
	target = (Uint32*)surface->pixels + y*surface->pitch / 4 + x;
	// Change color.
	(*target) = color;
	// unlock surface if needed
	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
}

void fill_rect(SDL_Renderer *renderer,
	const int x, const int y,
	const int width, const int height,
	const SDL_Color color)
{
	SDL_Rect rect = { x, y, width, height };
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, &rect);
}

void hollow_rect(SDL_Renderer *renderer,
	const int x, const int y,
	const int width, const int height,
	const SDL_Color color)
{
	SDL_Rect rect = { x, y, width, height };
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderDrawRect(renderer, &rect);
}
