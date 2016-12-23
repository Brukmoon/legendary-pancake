#include <stdlib.h>
#include "camera.h"
#include "collision.h"
#include "common.h"
#include "graphics.h"
#include "level.h"
#include "player.h"
#include "texture.h"
#ifdef _DEBUG
#include "game.h"
#endif // _DEBUG

#define ACTOR_TEXTURE "data/player.png"
#define ACTOR_STANDARD_SPEED 6
struct actor g_player;

void init_actor(struct actor *actor, SDL_Renderer *renderer)
{
	actor->texture = load_texture(renderer, ACTOR_TEXTURE);
	actor->skeleton.x = actor->skeleton.y = 32;
	actor->skeleton.w = actor->skeleton.h = 32;
	actor->x_vel = actor->y_vel = 0;
	actor->state = GROUND;
	actor->speed_coeff = ACTOR_STANDARD_SPEED;
	set_camera(&g_camera, (struct vec2) { actor->skeleton.x - CENTER_X, actor->skeleton.y - CENTER_Y });
}

void draw_actor(const struct actor *actor, SDL_Renderer *renderer)
{
	SDL_Rect dest;
	SDL_QueryTexture(actor->texture, NULL, NULL, &dest.w, &dest.h);
	dest.x = actor->skeleton.x - g_camera.position.x;
	dest.y = actor->skeleton.y - g_camera.position.y;
	SDL_RenderCopy(renderer, actor->texture, NULL, &dest);
}

void move_actor(struct actor *actor, float d_x, float d_y)
{
	SDL_Rect actor_after = { actor->skeleton.x, actor->skeleton.y + d_y,
		g_level->tile_map.tile_width, g_level->tile_map.tile_height };
	if (d_y != 0) // check y axis collision
	{
		if (!tilemap_collision(g_level, &actor_after))
			actor->skeleton.y += d_y;
		else
		{
			while (true)
			{
				if (d_y > 0)
					actor_after.y--;
				else
					actor_after.y++;
				if (!tilemap_collision(g_level, &actor_after))
					break;
			}
			actor->skeleton.y = actor_after.y;
		}
	}
	actor_after.x += d_x;
	if (d_x != 0) // check x axis collision
	{
		if (!tilemap_collision(g_level, &actor_after))
			actor->skeleton.x += d_x;
		else
		{
			while (true)
			{
				if (d_x > 0)
					actor_after.x--;
				else
					actor_after.x++;
				if (!tilemap_collision(g_level, &actor_after))
					break;
			}
			actor->skeleton.x = actor_after.x;
		}
	}
	set_camera(&g_camera, (struct vec2) { actor->skeleton.x - CENTER_X, actor->skeleton.y - CENTER_Y });
}