#include "common.h"
#include "button.h"
#include "text.h"

struct button* button_create(SDL_Renderer* renderer, struct button* parent, const char* text, const vec2 position)
{
	struct button* butt = malloc(sizeof(struct button));
	if (!butt)
	{
		ERROR("Button memory couldn't be allocated.");
		return NULL;
	}
	butt->texture = malloc(sizeof(SDL_Texture*) * BUTTON_SPRITE_COUNT);
	if (!butt->texture)
	{
		ERROR("Not enough memory!");
		return NULL;
	}

	butt->texture[BUTTON_SPRITE_INACTIVE] = create_text_texture(renderer, text, 25, (SDL_Color) { 0, 0, 0 });
	butt->texture[BUTTON_SPRITE_ACTIVE] = create_text_texture(renderer, text, 25, (SDL_Color) { 255, 0, 0 });
	butt->position = position;
	size_t str_len = SDL_strlen(text) + 1;
	butt->text = malloc(str_len);
	SDL_strlcpy(butt->text, text, str_len);
	butt->curr_sprite = BUTTON_SPRITE_INACTIVE; // inactive by default
	butt->next = NULL;
	butt->prev = parent;
	if (parent) // Parent exists.
	{
		parent->next = butt;
		INFO("Linked button [%d;%d] with button [%d;%d].", parent->position.x, parent->position.y, butt->position.x, butt->position.y);
	}
	INFO("Created button \"%s\" at position [%d;%d].", text, position.x, position.y);
	return butt;
}

void button_destroy(struct button *button)
{
	for (int i = 0; i < BUTTON_SPRITE_COUNT; ++i)
		SDL_DestroyTexture(button->texture[i]);
	free(button->texture);
	free(button->text);
	INFO("Button at position [%d;%d] destroyed.", button->position.x, button->position.y);
	free(button);
}

void set_button_active(struct button *button, bool yesno)
{
	button->curr_sprite = yesno?BUTTON_SPRITE_ACTIVE:BUTTON_SPRITE_INACTIVE;
	button->active = yesno;
}