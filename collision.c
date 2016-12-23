#include "collision.h"
#include "graphics.h"
#include "common.h"
#ifdef _DEBUG
#include "game.h"
#endif // _DEBUG
#include <SDL.h>

bool tilemap_collision(const struct level *level, const SDL_Rect* rect)
{
	if (rect == NULL || level == NULL)
	{
		INFO("Collision error.");
		return true;
	}
	int left = ((rect->x) / level->tile_map.tile_width) < 0?0:(rect->x) / level->tile_map.tile_width,
		right = ((rect->x + rect->w - 1) / level->tile_map.tile_width) > level->tile_map.width?level->tile_map.width - 1: (rect->x + rect->w - 1) / level->tile_map.tile_width,
		top = ((rect->y) / level->tile_map.tile_height) < 0?0: (rect->y) / level->tile_map.tile_height,
		bottom = (rect->y + rect->h - 1) / level->tile_map.tile_height > level->tile_map.height?level->tile_map.height - 1: (rect->y + rect->h - 1) / level->tile_map.tile_height;

	for (int i = left; i <= right; i++)
	{
		for (int j = top; j <= bottom; j++)
		{
			/*
			if (j > level->tile_map.height - 1 || i > level->tile_map.width - 1)
				break;
				*/
			tile t = level->tile_map.map[TMAP_COLLISION_LAYER][j][i];
			if (t != 0)
				return true;
		}
	}
	return false;
}