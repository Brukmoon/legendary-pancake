#include <stdlib.h>

#include "actor.h"
#include "camera.h"
#include "collision.h"
#include "common.h"
#include "graphics.h"
#include "level.h"
#include "sound.h"
#include "texture.h"

#define ACTOR_TEXTURE "data/player.png"
#define ACTOR_STANDARD_SPEED 6.f

struct actor g_player;

void actor_init(struct actor *actor, SDL_Renderer *renderer)
{
	actor->texture = load_texture(renderer, ACTOR_TEXTURE);
	actor->draw_state = 0;
	actor->sprite_count = 3;
	actor->skeleton.x = actor->skeleton.y = 32;
	actor->skeleton.w = actor->skeleton.h = 32;
	actor->velocity = (vec2f) { 0, 0 };
	actor->state = AIR;
	actor->speed = ACTOR_STANDARD_SPEED;
	actor->is_jumping = false;
	actor->jump_count = 0;
	camera_set(&g_camera, (vec2) { actor->skeleton.x - CENTER_X, actor->skeleton.y - CENTER_Y });
}

void actor_draw(const struct actor *actor, SDL_Renderer *renderer)
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

void actor_move(struct actor *actor, vec2 delta)
{
	// Where is the actor after the movement?
	SDL_Rect actor_after = { actor->skeleton.x + delta.x, actor->skeleton.y,
		g_level->tile_map.tile_width, g_level->tile_map.tile_height };
	// Handle each axis collision separately.
	if (delta.x != 0) // check x axis collision
	{
		if (!tilemap_collision(g_level, &actor_after)) // No collision, simple case.
			actor->skeleton.x += delta.x;
		else
		{
			while (true)
			{
				// Get as close as possible via stepping.
				// TODO: Is binary search feasible here?
				// No need, as long as I keep the speed <~100px/frame.
				if (delta.x > 0) // Moving right, --> collision from right.
					actor_after.x--;
				else // Collision from left.
					actor_after.x++;
				if (!tilemap_collision(g_level, &actor_after))
					break;
			}
			actor->skeleton.x = actor_after.x;
		}
		// TODO: Move into update_actor_draw_state.
		if (actor->state != AIR)
		{
			if (actor->draw_state < actor->sprite_count - 1)
				actor->draw_state += .2f;
			else
				actor->draw_state = 0;
		}
	}
	actor_after.y += delta.y;
	if (delta.y != 0) // check y axis collision
	{
		if (!tilemap_collision(g_level, &actor_after))
		{
			actor->state = AIR;
			actor->skeleton.y += delta.y;
		}
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
			// TODO: This is actually one frame late. Is it noticable?
			if (actor_after.y == actor->skeleton.y) // If the position after move hasn't changed.
			{
				if (actor->state != GROUND)
				{
					actor->state = GROUND;
				}
			}
			else // if it has, he must be in air
				actor->state = AIR;
			actor->skeleton.y = actor_after.y;
		}
	}
	
	// TODO: Move into update_double_jump.
	/*
	 * The two following functions should be called only if actor is PC --> maybe add a bool into the struct.
	 *
	 **/
	if (actor->is_jumping && actor->state == GROUND)
	{
		actor->is_jumping = false;
		actor->jump_count = 0;
	}
	camera_set(&g_camera, (vec2) { actor->skeleton.x - CENTER_X, actor->skeleton.y - CENTER_Y });
}

void actor_jump(struct actor *actor, float speed)
{
	/*
	 * To jump, actor must be either not jumping and on ground or jumping and meeting jump count requirement.
	 * TODO: Maybe make the engine more flexible by making double jump optional?
	 *
	 */
	if ((!actor->is_jumping && actor->state != AIR) || (actor->is_jumping && actor->jump_count < JUMP_COUNT))
	{
		actor->is_jumping = true;
		actor->jump_count++;
		sound_play("jump");
		// Go against gravity.
		actor->velocity.y = -speed;
	}
}