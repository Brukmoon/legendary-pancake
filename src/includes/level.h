#ifndef LEVEL_H
#define LEVEL_H

#include <SDL.h>

#include "map.h"

// Max length of level name.
#define LEVEL_NAME_LENGTH 30

struct level;

// Sets level#id as curr_level.
bool level_load(const char *name, SDL_Renderer *renderer);
// Saves level to file.
bool level_save();
// Must be called @ exit. Should be called when changing level.
void level_clean(struct level **level);

extern struct level
{
	// Level id.
	unsigned id;
	// Name of the level.
	char name[LEVEL_NAME_LENGTH];
	struct tile_map tile_map;
	// Map tileset.
	SDL_Texture *tileset;
	// Level background.
	SDL_Texture* background;
} *g_level;

#endif // LEVEL_H