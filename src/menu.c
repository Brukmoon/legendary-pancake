#include "camera.h"
#include "common.h"
#include "config.h"
#include "sound.h"
#include "text.h"
#include "graphics.h"
#include "menu.h"

void menu_load(SDL_Renderer* renderer)
{
	if (g_menu)
	{
		ERROR("Previous menu not freed, there cannot be two current menus. Free memory!");
		menu_destroy();
	}
	g_menu = malloc(sizeof(struct menu));
	if (!g_menu)
	{
		ERROR("Not enough memory!");
		return;
	}
	g_menu->button_count = 0;
	g_menu->button_list.root = g_menu->button_list.head = NULL;
	INFO("Menu memory allocated.");
	button_add(renderer, M_MENU_PLAY, (const vec2) { CENTER_X-2*FONT_SIZE, CENTER_Y });
	button_add(renderer, M_MENU_EDIT, (const vec2) { CENTER_X-(int)(2.5*FONT_SIZE), CENTER_Y+FONT_SIZE });
	button_add(renderer, M_MENU_QUIT, (const vec2) { CENTER_X-2*FONT_SIZE, CENTER_Y+2*FONT_SIZE });
	g_menu->button_list.current = g_menu->button_list.root;
	g_menu->button_list.root->curr_sprite = BUTTON_SPRITE_ACTIVE;
	g_menu->background = load_texture(renderer, IMG_PATH"background1.jpg");
}

void menu_destroy(void)
{
	if (!g_menu)
	{
		ERROR("Menu can't be freed, because it's NULL.");
		return;
	}
	struct button *temp = NULL;
	while (g_menu->button_list.root)
	{
		temp = g_menu->button_list.root;
		g_menu->button_list.root = g_menu->button_list.root->next;
		button_destroy(temp);
	}
	SDL_DestroyTexture(g_menu->background);
	free(g_menu);
	INFO("Menu memory freed.");
	INFO("Menu destroyed.");
}

void button_add(SDL_Renderer* renderer, const char* text, const vec2 position)
{
	if (!g_menu)
	{
		ERROR("Can't add button, menu memory not allocated. Call load_menu first.");
		return;
	}
	struct button *new_button = button_create(renderer, g_menu->button_list.head, text, position);
	g_menu->button_list.head = new_button;
	if (!g_menu->button_list.root)
		g_menu->button_list.root = new_button;
	INFO("Button %s created.", text);
	g_menu->button_count++;
}

void menu_draw(struct menu *menu, SDL_Renderer* renderer)
{
	if(menu->background)
		SDL_RenderCopy(renderer, menu->background, NULL, NULL);
	SDL_Rect dest = { 0, 0, 0, 0 };
	struct button* iterator = menu->button_list.root;
	while (iterator)
	{
		SDL_QueryTexture(iterator->texture[iterator->curr_sprite], NULL, NULL, &dest.w, &dest.h);
		dest.x = iterator->position.x;
		dest.y = iterator->position.y;
		SDL_RenderCopy(renderer, iterator->texture[iterator->curr_sprite], NULL, &dest);
		iterator = iterator->next;
	}
}

void menu_prev_button(struct menu *menu)
{
	sound_play("select");
	if (menu->button_list.current->prev)
	{
		set_button_active(menu->button_list.current->prev, true);
		set_button_active(menu->button_list.current, false);
		menu->button_list.current = menu->button_list.current->prev;
	}
	else
	{
		set_button_active(menu->button_list.head, true);
		set_button_active(menu->button_list.current, false);
		menu->button_list.current = menu->button_list.head;
	}
}

void menu_next_button(struct menu *menu)
{
	sound_play("select");
	if (menu->button_list.current->next)
	{
		set_button_active(menu->button_list.current->next, true);
		set_button_active(menu->button_list.current, false);
		menu->button_list.current = menu->button_list.current->next;
	}
	else
	{
		set_button_active(menu->button_list.root, true);
		set_button_active(menu->button_list.current, false);
		menu->button_list.current = menu->button_list.root;
	}
}