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
	actor->draw_state = 0;
	actor->sprite_count = 3;
	actor->skeleton.x = actor->skeleton.y = 32;
	actor->skeleton.w = actor->skeleton.h = 32;
	actor->velocity = (vec2f) { 0, 0 };
	actor->state = AIR;
	actor->speed = ACTOR_STANDARD_SPEED;
	set_camera(&g_camera, (vec2) { actor->skeleton.x - CENTER_X, actor->skeleton.y - CENTER_Y });
}

void draw_actor(const struct actor *actor, SDL_Renderer *renderer)
{
	SDL_Rect dest;
	dest.w = dest.h = 34; // Draw a bit larger.
	dest.x = actor->skeleton.x - g_camera.position.x;
	dest.y = actor->skeleton.y - g_camera.position.y;
	SDL_Rect src;
	src.w = src.h = 32;
	if (actor->velocity.x == 0) // If standing.
	{
		src.y = src.x = 0;
		SDL_RenderCopy(renderer, actor->texture, &src, &dest);
	}
	else // moving
	{
		src.x = ((int)actor->draw_state)*actor->skeleton.w;
		src.y = 96;
		if (actor->velocity.x < 0) // Moving left.
			SDL_RenderCopyEx(renderer, actor->texture, &src, &dest, 0, 0, SDL_FLIP_HORIZONTAL);
		else
			SDL_RenderCopy(renderer, actor->texture, &src, &dest);
	}
}

void move_actor(struct actor *actor, vec2 delta)
{
	SDL_Rect actor_after = { actor->skeleton.x, actor->skeleton.y + delta.y,
		g_level->tile_map.tile_width, g_level->tile_map.tile_height };
	actor->state = GROUND;
	if (delta.y != 0) // check y axis collision
	{
		if (!tilemap_collision(g_level, &actor_after))
			actor->skeleton.y += delta.y;
		else
		{
			actor->velocity.y = 0;
			while (true)
			{
				if (delta.y > 0)
					actor_after.y--;
				else
					actor_after.y++;
				if (!tilemap_collision(g_level, &actor_after))
					break;
			}
			actor->skeleton.y = actor_after.y;
		}
		if(actor_after.y != actor->skeleton.y)
			actor->state = AIR;
	}
	actor_after.x += delta.x;
	if (delta.x != 0) // check x axis collision
	{
		if (!tilemap_collision(g_level, &actor_after))
		{
			actor->skeleton.x += delta.x;
		}
		else
		{
			while (true)
			{
				if (delta.x > 0)
					actor_after.x--;
				else
					actor_after.x++;
				if (!tilemap_collision(g_level, &actor_after))
					break;
			}
			actor->skeleton.x = actor_after.x;
		}
		if (actor->state != AIR)
		{
			if (actor->draw_state < actor->sprite_count - 1)
				actor->draw_state += .2f;
			else
				actor->draw_state = 0;
		}
	}
	set_camera(&g_camera, (vec2) { actor->skeleton.x - CENTER_X, actor->skeleton.y - CENTER_Y });
}

void jump_actor(struct actor *actor, float speed)
{
	if (actor->state != AIR)
		actor->velocity.y = -speed;
}