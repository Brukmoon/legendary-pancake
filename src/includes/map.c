#include <stdlib.h>

#include "common.h"
#include "map.h"

bool tile_map_shrink(struct tile_map *map, enum expansion_type t, int value)
{
	switch (t)
	{
	case EXPAND_HEIGHT:
		if (value != 0)
		{
			map->height -= value;
			for (int j = 0; j < TMAP_LAYER_COUNT; ++j)
				map->map[j] = realloc(map->map[j], map->height * sizeof(tile*));
		}
		break;
	case EXPAND_WIDTH:
		if (value != 0)
		{
			map->width -= value;
			for (int j = 0; j < TMAP_LAYER_COUNT; ++j)
				for (int i = 0; i < map->height; ++i)
					map->map[j][i] = realloc(map->map[j][i], map->width * sizeof(tile));
		}
		break;
	default:
		ERROR("Unknown expansion type %d.", t);
		return false;
		break;
	}
	return true;
}

bool tile_map_expand(struct tile_map *map, enum expansion_type t, int value)
{
	switch (t)
	{
	case EXPAND_HEIGHT:
		if (value > 0)
		{
			map->height += value;
			for (int j = 0; j < TMAP_LAYER_COUNT; ++j)
			{
				map->map[j] = realloc(map->map[j], map->height * sizeof(tile*));
				for (int i = map->height - 1; i >= map->height - value; --i)
				{
					map->map[j][i] = malloc(map->width * sizeof(tile));
					for (int k = 0; k < map->width; ++k)
						map->map[j][i][k] = 0;
				}
			}
		}
		else if (value < 0)
		{
			INFO("Didn't you want to use tile_map_shrink instead?");
			return tile_map_shrink(map, t, value);
		}
		break;
	case EXPAND_WIDTH:
		if (value > 0)
		{
			map->width += value;
			for (int j = 0; j < TMAP_LAYER_COUNT; ++j)
				for (int i = 0; i < map->height; ++i)
				{
					map->map[j][i] = realloc(map->map[j][i], map->width * sizeof(tile));
					for (int k = map->width - 1; k >= map->width - value; --k)
						map->map[j][i][k] = 0;
				}
		}
		else if (value < 0)
		{
			INFO("Didn't you want to use tile_map_shrink instead?");
			return tile_map_shrink(map, t, value);
		}
		break;
	default:
		ERROR("Unknown expansion type %d.", t);
		return false;
		break;
	}
	return true;
	/*int *newpointer = realloc(map->map, 1000);
	if (newpointer == NULL) {
		/* problems!!!!                                 */
		/* tell the user to stop playing DOOM and retry */
		/* or free(oldpointer) and abort, or whatever   */
	//}
	//else {
		/* everything ok                                                                 */
		/* `newpointer` now points to a new memory block with the contents of oldpointer */
		/* `oldpointer` points to an invalid address                                     */
		//oldpointer = newpointer;
		/* oldpointer points to the correct address                                */
		/* the contents at oldpointer have been copied while realloc did its thing */
		/* if the new size is smaller than the old size, some data was lost        */
	//}
}