#ifndef LEVEL_H
#define LEVEL_H

#include <SDL.h>

#include "map.h"
#include "vector.h"

// Max length of level name.
#define LEVEL_NAME_LENGTH 30

struct level;

// Sets level#id as curr_level.
bool level_load(const char *name, SDL_Renderer *renderer);
// Saves level to file.
bool level_save(void);
// Must be called @ exit. Should be called when changing level.
void level_clean(void);

extern struct level
{
	// Level id.
	unsigned id;
	// Name of the level.
	char name[LEVEL_NAME_LENGTH];
	char next[LEVEL_NAME_LENGTH];
	// goal position
	vec2 goal;
	struct tile_map tile_map;
	// Map tileset.
	SDL_Texture *tileset;
	// Level background.
	SDL_Texture* background;
	// Dynamic background /*optional*/
	SDL_Texture* d_background;
} *g_level;

vec2 real_to_map(coord x, coord y);
#endif // LEVEL_H
