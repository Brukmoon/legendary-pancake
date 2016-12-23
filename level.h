#ifndef LEVEL_H
#define LEVEL_H

#include <SDL.h>

// Max length of level name.
#define LEVEL_NAME_LENGTH 30
#define TILE_SIZE 30

struct level;

// Sets level#id as curr_level.
bool level_load(int id, SDL_Renderer *renderer);
// Must be called @ exit. Should be called when changing level.
void level_clean(struct level **level);

// Representation of a tile, integer for now.
typedef int tile;
/*struct tile
{
	int id;
	...
};*/

enum tile_map_layer
{
	TMAP_TEXTURE_LAYER,
	TMAP_COLLISION_LAYER,
	TMAP_LAYER_COUNT
};

extern struct level
{
	unsigned id;
	char name[LEVEL_NAME_LENGTH];
	SDL_Texture* background;
	// TODO: Implement sounds.
	// TODO: Build layered map.
	struct
	{
		tile **map[TMAP_LAYER_COUNT];
		int width, height;
		int tile_width, tile_height;
	} tile_map;
	struct
	{
		int count;
		SDL_Texture **container;
	} textures;
} *g_level;

#endif // LEVEL_H