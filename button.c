#include "common.h"
#include "button.h"
#include "text.h"

struct button* create_button(SDL_Renderer* renderer, struct button* parent, const char* text, const struct vec2 position)
{
	struct button* butt = malloc(sizeof(struct button));
	if (!butt)
	{
		ERROR("Button memory couldn't be allocated.");
		return NULL;
	}
	butt->texture = malloc(sizeof(SDL_Texture*) * 2);
	butt->texture[BUTTON_SPRITE_MOUSE_OUT] = create_text_texture(renderer, text, 25, (SDL_Color) { 0, 0, 0 });
	butt->texture[BUTTON_SPRITE_MOUSE_OVER] = create_text_texture(renderer, text, 25, (SDL_Color) { 255, 0, 0 });
	butt->position = position;
	butt->curr_sprite = BUTTON_SPRITE_MOUSE_OUT;
	butt->next = NULL;
	if (parent) // Parent exists.
	{
		parent->next = butt;
		INFO("Linked button [%d;%d] with button [%d;%d].", parent->position.x, parent->position.y, butt->position.x, butt->position.y);
	}
	INFO("Created button \"%s\" at position [%d;%d].", text, position.x, position.y);
	return butt;
}

void destroy_button(struct button *button)
{
	for (int i = 0; i < BUTTON_SPRITE_COUNT; ++i)
		SDL_DestroyTexture(button->texture[i]);
	free(*button->texture);
	INFO("Button at position [%d;%d] destroyed.", button->position.x, button->position.y);
	free(button);
}