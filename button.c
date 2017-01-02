#include "common.h"
#include "button.h"
#include "sound.h"
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
	SDL_strlcpy(butt->text, text, BUTTON_TEXT_LENGTH);
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
	free(*button->texture);
	INFO("Button at position [%d;%d] destroyed.", button->position.x, button->position.y);
	free(button);
}

void button_active(struct button *button, struct button *prev_button)
{
	sound_play("select");
	button->curr_sprite = BUTTON_SPRITE_ACTIVE;
	prev_button->curr_sprite = BUTTON_SPRITE_INACTIVE;
}