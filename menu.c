#include "camera.h"
#include "common.h"
#include "text.h"
#include "graphics.h"
#include "menu.h"

void load_menu(SDL_Renderer* renderer)
{
	if (curr_menu)
	{
		ERROR("Previous menu not freed, there cannot be two current menus.");
		destroy_menu();
	}
	curr_menu = malloc(sizeof(struct menu));
	curr_menu->button_count = 0;
	curr_menu->button_list.root = curr_menu->button_list.head = NULL;
	INFO("Menu memory allocated.");
	add_button(renderer, "PLAY", (const vec2) { CENTER_X-2*FONT_SIZE, CENTER_Y });
	add_button(renderer, "EDITOR[NYI]", (const vec2) { CENTER_X-(int)(2.5*FONT_SIZE), CENTER_Y+FONT_SIZE });
	add_button(renderer, "QUIT", (const vec2) { CENTER_X-2*FONT_SIZE, CENTER_Y+2*FONT_SIZE });
	curr_menu->button_list.root->curr_sprite = BUTTON_SPRITE_MOUSE_OVER;
	curr_menu->background = load_texture(renderer, "data/background1.png");
}

void destroy_menu()
{
	if (!curr_menu)
	{
		ERROR("Menu can't be freed, because it's NULL.");
		return;
	}
	struct button *temp = NULL;
	while (curr_menu->button_list.head)
	{
		temp = curr_menu->button_list.head;
		curr_menu->button_list.head = curr_menu->button_list.head->next;
		free(temp);
	}
	SDL_DestroyTexture(curr_menu->background);
	free(curr_menu);
	INFO("Menu memory freed.");
	INFO("Menu destroyed.");
}

void add_button(SDL_Renderer* renderer, const char* text, const vec2 position)
{
	if (!curr_menu)
	{
		ERROR("Can't add button, menu memory not allocated. Call load_menu first.");
		return;
	}
	struct button *new_button = create_button(renderer, curr_menu->button_list.head, text, position);
	curr_menu->button_list.head = new_button;
	if (!curr_menu->button_list.root)
		curr_menu->button_list.root = new_button;
	INFO("Button %s created.", text);
	curr_menu->button_count++;
}

void draw_menu(SDL_Renderer* renderer)
{
	SDL_Rect dest = { 0, 0, 0, 0 };
	struct button* iterator = curr_menu->button_list.root;
	while (iterator)
	{
		SDL_QueryTexture(iterator->texture[iterator->curr_sprite], NULL, NULL, &dest.w, &dest.h);
		dest.x = iterator->position.x;
		dest.y = iterator->position.y;
		SDL_RenderCopy(renderer, iterator->texture[iterator->curr_sprite], NULL, &dest);
		iterator = iterator->next;
	}
}