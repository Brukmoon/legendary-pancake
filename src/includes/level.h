#ifndef LEVEL_H
#define LEVEL_H

#include <SDL.h>

// Max length of level name.
#define LEVEL_NAME_LENGTH 30

struct level;

// Sets level#id as curr_level.
bool level_load(int id, SDL_Renderer *renderer);
// Saves level to file.
bool level_save();
// Must be called @ exit. Should be called when changing level.
void level_clean(struct level **level);

// Representation of a tile, integer for now.
typedef int tile;
/*struct tile
{
	int id;
	...
};*/

// Map layers.
enum tile_map_layer
{
	TMAP_TEXTURE_LAYER,
	TMAP_COLLISION_LAYER,
	TMAP_LAYER_COUNT
};

extern struct level
{
	// Level id.
	unsigned id;
	// Name of the level.
	char name[LEVEL_NAME_LENGTH];
	// Level background.
	SDL_Texture* background;
	// TODO: Implement sounds.
	struct
	{
		tile **map[TMAP_LAYER_COUNT];
		int width, height;
		int tile_width, tile_height;
	} tile_map;
	// Map tileset.
	SDL_Texture *tileset;
} *g_level;

#endif // LEVEL_H