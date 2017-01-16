#ifndef MENU_H
#define MENU_H

#include <SDL.h>

#include "button.h"
#include "stack.h"

#define M_MENU_PLAY "PLAY"
#define M_MENU_EDIT "EDITOR"
#define M_MENU_QUIT "QUIT"

struct menu
{
	struct
	{
		struct button *root;
		// Keep head in memory for quick access.
		struct button *head;
		// Pointer to the button currently active.
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