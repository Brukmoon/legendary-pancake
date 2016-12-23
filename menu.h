#ifndef MENU_H
#define MENU_H

#include "button.h"

struct menu
{
	struct
	{
		struct button *root;
		struct button *head;
	} button_list;
	int max_button_count;
	int button_count;
} *curr_menu;

void add_button(SDL_Renderer* renderer, const char* text, const struct vec2 position);

void load_menu(SDL_Renderer* renderer);
void draw_menu(SDL_Renderer* renderer);
void destroy_menu();

#endif // MENU_H