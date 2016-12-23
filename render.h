/*
 * Draw functions.
 * @author Michal H.
 *
 **/
#ifndef RENDER_H
#define RENDER_H

#include <SDL.h>

// Draws the curr_map map to the screen.
void render_map(SDL_Renderer* const renderer);

// Draws the main menu.
void render_menu_interface(SDL_Renderer* const renderer);

// State callbacks. %callback_%statename
void render_menu(SDL_Renderer *renderer);
void render_play(SDL_Renderer *renderer);
void render_edit(SDL_Renderer *renderer);

#endif // RENDER_H