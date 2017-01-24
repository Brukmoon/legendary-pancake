#ifndef SPRITE_H
#define SPRITE_H

#define SPRITE_ARR_SIZE 20
#define TEXTURE_ARR_SIZE 5

#include <SDL.h>

struct texture_bucket
{
	struct texture_bucket *next;
	char* key;
	SDL_Texture* texture;
};

static struct
{
	struct texture_bucket *data[TEXTURE_ARR_SIZE];
} textures_container;

const char* texture_add(const char* name, SDL_Renderer *renderer);
static SDL_Texture* texture_get(const char* name);
void textures_destroy(void);

// Level texture data --> objects, actors (enemies) etc.
struct sprite_bucket
{
	struct sprite_bucket *next;
	char* key;

	struct {
		// If NULL, full.
		SDL_Rect *target;
		char* texture;
	} sprite;
};

static struct
{
	struct sprite_bucket *data[SPRITE_ARR_SIZE];
} sprites_container;

void sprite_add(const char* name, const char* texture_name, const SDL_Rect target);
SDL_Texture *sprite_get(const char* name, SDL_Rect** target);
void sprites_destroy(void);

#endif // SPRITE_H