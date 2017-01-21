#include <stdlib.h>

#include "common.h"
#include "map.h"

static bool tile_map_resize_width(struct tile_map *map, int value);
static bool tile_map_resize_height(struct tile_map *map, int value);

bool tile_map_resize(struct tile_map *map, enum resize_type t, int value)
{
	switch (t)
	{
	case RESIZE_HEIGHT:
		return tile_map_resize_height(map, value);
		break;
	case RESIZE_WIDTH:
		return tile_map_resize_width(map, value);
		break;
	default:
		ERROR("Unknown expansion type %d.", t);
		return false;
		break;
	}
}

static bool tile_map_resize_width(struct tile_map *map, int value)
{
	// shrink
	if (value < 0)
	{
		map->width += value;
		for (int j = 0; j < TMAP_LAYER_COUNT; ++j)
			// add +value tile to each row in each layer
			for (int i = 0; i < map->height; ++i)
			{
				tile *m = realloc(map->map[j][i], map->width * sizeof(tile));
				if (!m)
				{
					ERROR("Not enough memory!");
					return false;
				}
				map->map[j][i] = m;
			}
	}
	// expand
	else if (value > 0)
	{
		map->width += value;
		for (int j = 0; j < TMAP_LAYER_COUNT; ++j)
			for (int i = 0; i < map->height; ++i)
			{
				tile *m = realloc(map->map[j][i], map->width * sizeof(tile));
				if (!m)
				{
					ERROR("Not enough memory!");
					return false;
				}
				map->map[j][i] = m;
				for (int k = map->width - 1; k >= map->width - value; --k)
					map->map[j][i][k] = 0;
			}
	}
	return true;
}

static bool tile_map_resize_height(struct tile_map *map, int value)
{
	if (value < 0)
	{
		map->height += value;
		for (int j = 0; j < TMAP_LAYER_COUNT; ++j)
		{
			tile** t = realloc(map->map[j], map->height * sizeof(tile*));
			if (!*t)
			{
				ERROR("Not enough memory!");
				return false;
			}
			map->map[j] = t;
		}
	}
	else if (value > 0)
	{
		map->height += value;
		for (int j = 0; j < TMAP_LAYER_COUNT; ++j)
		{
			tile** t = realloc(map->map[j], map->height * sizeof(tile*));
			if (!*t)
			{
				ERROR("Not enough memory!");
				return false;
			}
			map->map[j] = t;
			for (int i = map->height - 1; i >= map->height - value; --i)
			{
				tile* m = malloc(map->width * sizeof(tile));
				if (!m)
				{
					ERROR("Not enough memory!");
					return false;
				}
				map->map[j][i] = m;
				for (int k = 0; k < map->width; ++k)
					map->map[j][i][k] = 0;
			}
		}
	}
	return true;
}