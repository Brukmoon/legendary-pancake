#ifndef BUTTON_H
#define BUTTON_H

#include "texture.h"
#include "position.h"

enum button_sprite
{
	BUTTON_SPRITE_MOUSE_OUT,
	BUTTON_SPRITE_MOUSE_OVER,
	BUTTON_SPRITE_COUNT
};

struct button
{
	enum button_sprite curr_sprite;
    vec2 position;
	SDL_Texture **texture;

	struct button* next;
};

struct button* create_button(SDL_Renderer* renderer, struct button* parent, const char* text, const vec2 position);
void destroy_button(struct button *button);

#endif // BUTTON_H