#ifndef BUTTON_H
#define BUTTON_H

#include "texture.h"
#include "vector.h"

// Texture types.
enum button_sprite
{
	BUTTON_SPRITE_INACTIVE,
	BUTTON_SPRITE_ACTIVE,
	BUTTON_SPRITE_COUNT
};

struct button
{
	enum button_sprite curr_sprite;
    vec2 position;
	// Every button has unique textures.
	SDL_Texture **texture;
	// Linking buttons.
	struct button* next;
	struct button* prev;
};

struct button* button_create(SDL_Renderer* renderer, struct button* parent, const char* text, const vec2 position);
void button_destroy(struct button *button);
// Set button as active.
void button_active(struct button *button, struct button *prev_button);

#endif // BUTTON_H