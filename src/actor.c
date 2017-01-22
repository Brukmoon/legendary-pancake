#include <stdlib.h>

#include "actor.h"
#include "camera.h"
#include "collision.h"
#include "common.h"
#include "config.h"
#include "graphics.h"
#include "level.h"
#include "sound.h"
#include "texture.h"

#define PLAYER_TEXTURE IMG_PATH"player.png"

struct player g_player;

void actor_init(struct actor *actor)
{
	// default draw state
	actor->draw_state = 0;
	SDL_strlcpy(actor->name, "player", ACTOR_NAME_MAX_LENGTH);
	actor->hitpoints = ACTOR_HP;
	actor->sprite_count = 3;
	actor->skeleton.x = actor->skeleton.y = 32;
	actor->skeleton.w = actor->skeleton.h = 32;
	actor->velocity = (vec2f) { 0, 0 };
	actor->is_visible = false;
	actor->is_jumping = false;
	actor->state = AIR;
	actor->speed = ACTOR_STANDARD_SPEED;
	actor->jump_count = 0;
	camera_set(&g_camera, (vec2) { actor->skeleton.x - CENTER_X, actor->skeleton.y - CENTER_Y });
}

void actor_draw(const struct actor *actor, SDL_Renderer *renderer)
{
	UNUSED_PARAMETER(actor);
	UNUSED_PARAMETER(renderer);
}

void actor_move(struct actor *actor, const vec2 delta)
{
	// Where is the actor after the movement?
	SDL_Rect actor_after = { actor->skeleton.x + delta.x, actor->skeleton.y,
		actor->skeleton.w, actor->skeleton.h };
	// Handle each axis collision separately.
	if (delta.x != 0) // check x axis collision
	{
		if (!tilemap_collision(g_level, &actor_after, TILE_COLLISION)) // No collision, simple case.
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
				if (!tilemap_collision(g_level, &actor_after, TILE_COLLISION))
					break;
			}
			actor->skeleton.x = actor_after.x;
		}
	}
	actor_after.y += delta.y;
	if (delta.y != 0) // check y axis collision
	{
		if (!tilemap_collision(g_level, &actor_after, TILE_COLLISION))
		{
			if(!tilemap_collision(g_level, &actor_after, LADDER_COLLISION))
				actor->state = AIR;
			actor->skeleton.y += delta.y;
		}
		else
		{
			while (true)
			{
				if (delta.y > 0)
					actor_after.y--;
				else
					actor_after.y++;
				if (!tilemap_collision(g_level, &actor_after, TILE_COLLISION))
					break;
			}
			if (actor_after.y >= actor->skeleton.y) // If the position after move hasn't changed.
			{
				if (actor->state == AIR)
				{
					if (actor->velocity.y > 12)
						actor_damage(actor, ((int)actor->velocity.y % 10)*10);
				}
			}
			actor->state = GROUND;
			actor->velocity.y = 0;
			actor->skeleton.y = actor_after.y;
		}
	}
}

void actor_damage(struct actor *actor, const Sint16 damage)
{
	sound_play("fall");
#if DAMAGE_ON
	if (actor->hitpoints - damage > 0) // Hitpoints can't be less than 0.
		actor->hitpoints -= damage;
	else
		actor->hitpoints = 0;
#endif // GOD_MODE
}

bool actor_jump(struct actor *actor, float speed)
{
	// To jump, actor must be either not jumping and on ground or jumping and meeting jump count requirement.
	if ((!actor->is_jumping && actor->state == GROUND) || (actor->is_jumping && actor->jump_count < MULTI_JUMP))
	{
		// He is jumping until he hits the ground.
		actor->is_jumping = true;
		actor->jump_count++;
		// Go against gravity.
		actor->velocity.y = -speed;
		return true;
	}
	return false;
}

void actor_spawn(struct actor *actor)
{
	actor->skeleton.x = actor->spawn.x;
	actor->skeleton.y = actor->spawn.y;
	INFO("Actor %s spawned at [%d;%d].", actor->name, actor->spawn.x, actor->spawn.y);
	actor->is_visible = true;
}

void player_init(struct player *player, SDL_Renderer *renderer)
{ 
	actor_init(&player->actor); 
	player->climb[0] = player->climb[1] = false;
	player->texture = load_texture(renderer, PLAYER_TEXTURE);
}

void player_draw(const struct player *player, SDL_Renderer *renderer)
{
	if (player->actor.is_visible)
	{
		SDL_Rect dest;
		dest.w = dest.h = 34; // Draw a bit larger.
		dest.x = player->actor.skeleton.x - g_camera.position.x;
		dest.y = player->actor.skeleton.y - g_camera.position.y;
		SDL_Rect src;
		src.w = src.h = 32;
		if (player->actor.state == LADDER)
		{
			src.x = ((int)player->actor.draw_state)*player->actor.skeleton.w;
			src.y = 64;
			SDL_RenderCopy(renderer, player->texture, &src, &dest);
		}
		else if (player->actor.velocity.x == 0) // If standing.
		{
			src.y = src.x = 0;
			SDL_RenderCopy(renderer, player->texture, &src, &dest);
		}
		else // moving
		{
			src.x = ((int)player->actor.draw_state)*player->actor.skeleton.w;
			src.y = 96;
			if (player->actor.velocity.x < 0) // Moving left.
				SDL_RenderCopyEx(renderer, player->texture, &src, &dest, 0, 0, SDL_FLIP_HORIZONTAL);
			else
				SDL_RenderCopy(renderer, player->texture, &src, &dest);
		}
	}
}

void player_move(struct player *player, const vec2 delta)
{
	actor_move(&player->actor, delta);
	if (delta.x != 0)
		; // Perhaps add some walk effect.
}

void player_jump(struct player *player, float speed) 
{ 
	if (actor_jump(&player->actor, speed))
		sound_play("jump");
}

void player_spawn(struct player *player)
{
	actor_spawn(&player->actor);
}

bool player_can_climb(struct player *player)
{
	return tilemap_collision(g_level, &player->actor.skeleton, LADDER_COLLISION);
}

void player_climb(struct player *player)
{
	player->actor.state = LADDER;
	if (player->climb[0])
		player_set_vel_y(player, CLIMB_SPEED);
	else if (player->climb[1])
		player_set_vel_y(player, -CLIMB_SPEED);
	else
		player_set_vel_y(player, 0);
}