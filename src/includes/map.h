#ifndef MAP_H
#define MAP_H

#include <stdbool.h>

// Representation of a tile bit.
/*
 * MAP w h
 * 0:0 0:0 ... 0:0
 * 0:0 0:0 ... 0:1
 *
 * texture_id:collision
 */
typedef int tile;

// Resize mode.
enum resize_type
{
	RESIZE_WIDTH,
	RESIZE_HEIGHT
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
	/*
	 * You can have multiple layers. For now, there are only two - collision and textures.
	 * But it would be possible to have background, foreground and collision layers.
	 * Player enters a "tunnel" --> becomes invisible.
	 *
	 */
	tile **map[TMAP_LAYER_COUNT];
	int width, height;
	int tile_width, tile_height;
};

// Expand or shrink the tile map.
bool tile_map_resize(struct tile_map *map, enum resize_type t, int value);

#endif // MAP_H