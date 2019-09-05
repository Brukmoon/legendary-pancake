/*
 * Graphic routines for drawing primitives on a surface.
 * @author Michal H.
 *
 */
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL.h>

// Set pixel color on surface at [x;y].
void put_pixel(SDL_Surface *surface,
	const int x, const int y,
	Uint8 r, Uint8 g, Uint8 b);
// Create a filled/hollow rectangle tied to the renderer.
void fill_rect(SDL_Renderer *renderer,
	const int x, const int y,
	const int width, const int height,
	const SDL_Color color);
void hollow_rect(SDL_Renderer* renderer,
	const int x, const int y,
	const int width, const int height,
	const SDL_Color color);

#endif // GRAPHICS_H