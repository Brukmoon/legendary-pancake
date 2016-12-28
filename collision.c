#include <SDL.h>

#include "collision.h"
#include "level.h"
#include "common.h"

bool tilemap_collision(const struct level *level, const SDL_Rect* rect)
{
	if (rect == NULL || level == NULL)
	{
		ERROR("Level or rect can't be NULL.");
		return true;
	}
	// Perform checking only for the tiles around the rectangle.
	int left = ((rect->x) / level->tile_map.tile_width) < 0?0:(rect->x) / level->tile_map.tile_width,
		right = ((rect->x + rect->w - 1) / level->tile_map.tile_width) > level->tile_map.width?level->tile_map.width - 1: (rect->x + rect->w - 1) / level->tile_map.tile_width,
		top = ((rect->y) / level->tile_map.tile_height) < 0?0: (rect->y) / level->tile_map.tile_height,
		bottom = (rect->y + rect->h - 1) / level->tile_map.tile_height > level->tile_map.height?level->tile_map.height - 1: (rect->y + rect->h - 1) / level->tile_map.tile_height;

	for (int i = left; i <= right; i++)
		for (int j = top; j <= bottom; j++)
		{
			/*
			if (j > level->tile_map.height - 1 || i > level->tile_map.width - 1)
				break;*/
				
			tile t = level->tile_map.map[TMAP_COLLISION_LAYER][j][i];
			/*
			 * t can be 0 or 1 [collision false or collision true].
			 *
			 */
			if (t != 0)
				return true;
		}
	return false;
}