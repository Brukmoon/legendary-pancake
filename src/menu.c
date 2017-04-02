#include "camera.h"
#include "common.h"
#include "config.h"
#include "sound.h"
#include "text.h"
#include "graphics.h"
#include "menu.h"

void menu_create(struct menu **menu, const enum menu_flags flags)
{
	if (g_menu)
	{
		INFO("Previous menu not freed. Cleaning up.");
		menu_destroy();
	}
	*menu = malloc(sizeof(struct menu));
	if (!g_menu)
	{
		ERROR("Not enough memory!");
		return;
	}
	if (flags & MENU_BUTTON)
	{
		g_menu->button_count = 0;
		g_menu->button_list = malloc(sizeof(struct button_list));
		if (!g_menu->button_list)
			ERROR("Not enough memory!");
		g_menu->button_list->root = g_menu->button_list->head = NULL;
	}
	else
		g_menu->button_list = NULL;
	if (flags & MENU_TEXT_BOX)
	{
		g_menu->text_box_list = malloc(sizeof(struct text_box_list));
		if (!g_menu->text_box_list)
			ERROR("Not enough memory!");
		g_menu->text_box_list->root = g_menu->text_box_list->head = NULL;
	}
	else
		g_menu->text_box_list = NULL;
}

void main_menu_load(SDL_Renderer* renderer)
{
	menu_create(&g_menu, MENU_BUTTON);
	button_add(renderer, M_MENU_PLAY, BUTTON_FONT_SIZE, (const vec2) { CENTER_X-20, CENTER_Y });
	button_add(renderer, M_MENU_EDIT, BUTTON_FONT_SIZE, (const vec2) { CENTER_X-37, CENTER_Y+BUTTON_FONT_SIZE});
	button_add(renderer, M_MENU_QUIT, BUTTON_FONT_SIZE, (const vec2) { CENTER_X-20, CENTER_Y+2*BUTTON_FONT_SIZE});
	g_menu->button_list->current = g_menu->button_list->root;
	g_menu->button_list->root->curr_sprite = BUTTON_SPRITE_ACTIVE;
	g_menu->background = load_texture(renderer, IMG_PATH"background1.png");
}

void preedit_menu_load(SDL_Renderer* renderer)
{
	menu_create(&g_menu, MENU_ALL);
	text_box_add((const SDL_Rect) { CENTER_X - 65, CENTER_Y, 130, BUTTON_FONT_SIZE + 10}, 10);
	button_add(renderer, MENU_OK, BUTTON_FONT_SIZE, (const vec2) { CENTER_X - 2*BUTTON_FONT_SIZE - 10, CENTER_Y + BUTTON_FONT_SIZE + 10 });
	button_add(renderer, MENU_CANCEL, BUTTON_FONT_SIZE,(const vec2) { CENTER_X, CENTER_Y + BUTTON_FONT_SIZE + 10 });
	g_menu->button_list->current = g_menu->button_list->root;
	g_menu->text_box_list->current = g_menu->text_box_list->root;
	g_menu->button_list->root->curr_sprite = BUTTON_SPRITE_ACTIVE;
	g_menu->background = load_texture(renderer, IMG_PATH"background1.png");
}

void preplay_menu_load(SDL_Renderer* renderer)
{
	menu_create(&g_menu, MENU_BUTTON);
	button_add(renderer, P_MENU_LEVEL1, BUTTON_FONT_SIZE, (const vec2) { CENTER_X - 10, CENTER_Y });
	button_add(renderer, P_MENU_LEVEL2, BUTTON_FONT_SIZE, (const vec2) { CENTER_X - 10, CENTER_Y + BUTTON_FONT_SIZE});
	button_add(renderer, MENU_CANCEL, BUTTON_FONT_SIZE, (const vec2) { CENTER_X - 8, CENTER_Y + 2*BUTTON_FONT_SIZE });
	g_menu->button_list->current = g_menu->button_list->root;
	g_menu->button_list->root->curr_sprite = BUTTON_SPRITE_ACTIVE;
	g_menu->background = load_texture(renderer, IMG_PATH"background1.png");
}

void preplay_difficulty_menu_load(SDL_Renderer* renderer)
{
	menu_create(&g_menu, MENU_BUTTON);
	button_add(renderer, P_DIFFICULTY_NORMAL, BUTTON_FONT_SIZE, (const vec2) { CENTER_X - 24, CENTER_Y });
	button_add(renderer, P_DIFFICULTY_EXTERMINATION, BUTTON_FONT_SIZE, (const vec2) { CENTER_X - 8, CENTER_Y + BUTTON_FONT_SIZE });
	button_add(renderer, P_DIFFICULTY_PACIFIC, BUTTON_FONT_SIZE, (const vec2) { CENTER_X - 24, CENTER_Y + 2 * BUTTON_FONT_SIZE });
	button_add(renderer, MENU_CANCEL, BUTTON_FONT_SIZE, (const vec2) { CENTER_X - 10, CENTER_Y + 3 * BUTTON_FONT_SIZE });
	g_menu->button_list->current = g_menu->button_list->root;
	g_menu->button_list->root->curr_sprite = BUTTON_SPRITE_ACTIVE;
	g_menu->background = load_texture(renderer, IMG_PATH"background1.png");
}

void menu_destroy(void)
{
	if (!g_menu)
	{
		ERROR("Menu can't be freed, because it's NULL.");
		return;
	}
	if (g_menu->button_list)
	{
		struct button *temp = NULL;
		while (g_menu->button_list->root)
		{
			temp = g_menu->button_list->root;
			g_menu->button_list->root = g_menu->button_list->root->next;
			button_destroy(temp);
		}
		free(g_menu->button_list);
	}
	if (g_menu->text_box_list)
	{
		struct text_box *temp = NULL;
		while (g_menu->text_box_list->root)
		{
			temp = g_menu->text_box_list->root;
			g_menu->text_box_list->root = g_menu->text_box_list->root->next;
			text_box_destroy(temp);
		}
		free(g_menu->text_box_list);
	}
	SDL_DestroyTexture(g_menu->background);
	free(g_menu);
	g_menu = NULL;
	INFO("Menu memory freed.");
	INFO("Menu destroyed.");
}

void button_add(SDL_Renderer* renderer, const char* text, size_t const font_size, const vec2 position)
{
	if (!g_menu)
	{
		ERROR("Can't add button, menu memory not allocated. Call load_menu first.");
		return;
	}
	if (!g_menu->button_list)
	{
		INFO("Button list not used. Allocating!");
		g_menu->button_list = malloc(sizeof(struct button_list));
		if (!g_menu->button_list)
		{
			ERROR("Not enough memory!");
			return;
		}
	}
	struct button *new_button = button_create(renderer, g_menu->button_list->head, text, font_size, position);
	g_menu->button_list->head = new_button;
	if (!g_menu->button_list->root)
		g_menu->button_list->root = new_button;
	INFO("Button %s created.", text);
	g_menu->button_count++;
}

void text_box_add(const SDL_Rect skeleton, int max_length)
{
	if (!g_menu)
	{
		ERROR("Can't add button, menu memory not allocated. Call load_menu first.");
		return;
	}
	if (!g_menu->text_box_list)
	{
		INFO("Textbox list not used. Allocating!");
		g_menu->text_box_list = malloc(sizeof(struct text_box_list));
		if (!g_menu->text_box_list)
		{
			ERROR("Not enough memory!");
			return;
		}
	}
	struct text_box *new_text_box = text_box_create(g_menu->text_box_list->head, skeleton, max_length);
	g_menu->text_box_list->head = new_text_box;
	if (!g_menu->text_box_list->root)
		g_menu->text_box_list->root = new_text_box;
	INFO("Textbox created.");
}

void menu_draw(struct menu *menu, SDL_Renderer* renderer)
{
	if(menu->background) // there is a background
		SDL_RenderCopy(renderer, menu->background, NULL, NULL);
	SDL_Rect dest = { 0, 0, 0, 0 };
	if (menu->button_list) // there are buttons
	{
		struct button* iterator = menu->button_list->root;
		while (iterator)
		{
			SDL_QueryTexture(iterator->texture[iterator->curr_sprite], NULL, NULL, &dest.w, &dest.h);
			dest.x = iterator->position.x;
			dest.y = iterator->position.y;
			SDL_RenderCopy(renderer, iterator->texture[iterator->curr_sprite], NULL, &dest);
			iterator = iterator->next;
		}
	}
	if (menu->text_box_list) // there are text boxes
	{
		struct text_box *iterator = menu->text_box_list->root;
		while (iterator)
		{
			hollow_rect(renderer, iterator->r.x, iterator->r.y, iterator->r.w, iterator->r.h, (SDL_Color){ 0, 0, 0, 1 });
			if(SDL_strlen(iterator->text) > 0)
				draw_text(iterator->text, BUTTON_FONT_SIZE, (SDL_Color) { 0, 0, 0, 1 }, (vec2) { iterator->r.x, iterator->r.y }, renderer);
			iterator = iterator->next;
		}
	}
}

void menu_prev_button(struct menu *menu)
{
	sound_play("select");
	if (menu->button_list->current->prev)
	{
		set_button_active(menu->button_list->current->prev, true);
		set_button_active(menu->button_list->current, false);
		menu->button_list->current = menu->button_list->current->prev;
	}
	else
	{
		set_button_active(menu->button_list->head, true);
		set_button_active(menu->button_list->current, false);
		menu->button_list->current = menu->button_list->head;
	}
}

void menu_next_button(struct menu *menu)
{
	sound_play("select");
	// is there next button on the list?
	if (menu->button_list->current->next)
	{
		set_button_active(menu->button_list->current->next, true);
		set_button_active(menu->button_list->current, false);
		menu->button_list->current = menu->button_list->current->next;
	}
	else
	{
		// start from beginning
		set_button_active(menu->button_list->root, true);
		set_button_active(menu->button_list->current, false);
		menu->button_list->current = menu->button_list->root;
	}
}