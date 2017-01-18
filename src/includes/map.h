#ifndef MAP_H
#define MAP_H

#include <stdbool.h>

// Representation of a tile, integer for now.
typedef int tile;

enum expansion_type
{
	EXPAND_WIDTH,
	EXPAND_HEIGHT
};

// Map layers.
enum tile_map_layer
{
	TMAP_TEXTURE_LAYER,
	TMAP_COLLISION_LAYER,
	TMAP_LAYER_COUNT
};

struct tile_map
{
	tile **map[TMAP_LAYER_COUNT];
	int width, height;
	int tile_width, tile_height;
};

bool tile_map_shrink(struct tile_map *map, enum expansion_type t, int value);
bool tile_map_expand(struct tile_map *map, enum expansion_type t, int value);

#endif // MAP_H