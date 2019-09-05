#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>

#include "texture.h"
#include "vector.h"

#define BUTTON_FONT_SIZE 35

// Texture types.
enum button_sprite
{
	BUTTON_SPRITE_INACTIVE,
	BUTTON_SPRITE_ACTIVE,
	BUTTON_SPRITE_COUNT
};

struct button
{
	bool active;
	enum button_sprite curr_sprite;
    vec2 position;
	char *text;
	// Every button has a unique textures.
	SDL_Texture **texture;
	// Linking buttons.
	struct button* next;
	struct button* prev;
};

// create and return a button
struct button* button_create(SDL_Renderer* renderer, struct button* parent, const char* text, size_t const font_size, const vec2 position);
void button_destroy(struct button *button);
// Set button as active.
void set_button_active(struct button *button, bool yesno);

#endif // BUTTON_H
