/*
 * Draw functions.
 * @author Michal H.
 *
 **/
#ifndef RENDER_H
#define RENDER_H

#include <SDL.h>

enum render_map_flags
{
	RENDER_BASE = 1 << 0,
	RENDER_GRID = 1 << 1,
	RENDER_COLL = 1 << 2,
	RENDER_ALL = RENDER_GRID | RENDER_COLL,
};

// Draws the curr_map map to the screen.
void render_map(SDL_Renderer* const renderer, const enum render_map_flags f);

// Draws the main menu.
void render_menu_interface(SDL_Renderer* const renderer);

// State callbacks. %callback_%statename
void render_menu(SDL_Renderer *renderer);
void render_play(SDL_Renderer *renderer);
void render_edit(SDL_Renderer *renderer);

#endif // RENDER_H