#ifndef MENU_H
#define MENU_H

#include <SDL.h>

#include "button.h"
#include "text_box.h"

#define M_MENU_PLAY "PLAY"
#define M_MENU_EDIT "EDITOR"
#define M_MENU_QUIT "QUIT"

#define MENU_OK "OK"
#define MENU_CANCEL "BACK"

#define P_MENU_LEVEL1 "level1"
#define P_MENU_LEVEL2 "demo"

#define P_DIFFICULTY_NORMAL "normal"
#define P_DIFFICULTY_EXTERMINATION "killer"
#define P_DIFFICULTY_PACIFIC "pacifist"

struct button_list
{
	struct button *root;
	// Keep head in memory for quick access.
	struct button *head;
	// Pointer to the button currently active.
	struct button *current;
};

struct text_box_list
{
	struct text_box* root;
	struct text_box* head, *current;
};

enum menu_flags
{
	MENU_BUTTON = 1 << 0,
	MENU_TEXT_BOX = 1 << 1,
	MENU_ALL = MENU_BUTTON | MENU_TEXT_BOX,
};

struct menu
{
	struct button_list* button_list;
	struct text_box_list *text_box_list;
	int button_count;
	SDL_Texture *background;
} *g_menu;

struct menu_level
{
	struct menu_level *next;
	struct menu_level *prev;
};

struct menu_level_list
{
	struct menu_level *head, *tail;
};

void button_add(SDL_Renderer* renderer, const char* text, size_t const font_size, const vec2 position);
void text_box_add(const SDL_Rect skeleton, int max_length);
void main_menu_load(SDL_Renderer* renderer);
void preedit_menu_load(SDL_Renderer* renderer);
void preplay_menu_load(SDL_Renderer* renderer);
void preplay_difficulty_menu_load(SDL_Renderer* renderer);

void menu_create(struct menu **menu, const enum menu_flags flags);
void menu_draw(struct menu *menu, SDL_Renderer* renderer);
void menu_destroy(void);
void menu_prev_button(struct menu *menu);
void menu_next_button(struct menu *menu);

#endif // MENU_H