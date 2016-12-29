#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include "button.h"

struct menu
{
	struct
	{
		struct button *root;
		// Keep head in memory for quick access.
		struct button *head;
		// Currently active button.
		struct button *current;
	} button_list;
	int max_button_count;
	int button_count;
	SDL_Texture *background;
} *g_menu;

void button_add(SDL_Renderer* renderer, const char* text, const vec2 position);

void menu_load(SDL_Renderer* renderer);
void menu_draw(struct menu *menu, SDL_Renderer* renderer);
void menu_destroy(void);
void menu_prev_button(struct menu *menu);
void menu_next_button(struct menu *menu);

#endif // MENU_H