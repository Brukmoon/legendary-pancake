#include "actor.h"
#include "camera.h"
#include "collision.h"
#include "common.h"
#include "config.h"
#include "level.h"
#include "sound.h"
#include "texture.h"

#define PLAYER_PATH IMG_PATH"player.png"

struct player g_player;

void actor_init(struct actor *actor, const char* name, const vec2 spawn, const char* anim_name, SDL_Renderer *renderer)
{
	actor->name = malloc(7);
	SDL_strlcpy(actor->name, name, 7);
	actor->hitpoints = ACTOR_HP;
	actor->skeleton.x = actor->skeleton.y = 0;
	actor->skeleton.w = actor->skeleton.h = 32;
	actor->velocity = (vec2f) { 0, 0 };
	actor->is_visible = actor->is_jumping = false;
	actor->state = AIR;
	actor->speed = ACTOR_STANDARD_SPEED;
	actor->jump_count = 0;
	actor->spawn = spawn;
	animation_table_load(anim_name, &actor->anim, renderer);
	animation_set("stand", &actor->anim);
}

void actor_destroy(struct actor *actor)
{
	free(actor->name);
	animation_table_destroy(&actor->anim);
}

// NYI
void actor_draw(const struct actor *actor, SDL_Renderer *renderer)
{
	UNUSED_PARAMETER(actor);
	UNUSED_PARAMETER(renderer);
}

void actor_move(struct actor *actor, const vec2* delta)
{
	// Where is the actor after the movement?
	SDL_Rect actor_after = { actor->skeleton.x + delta->x, actor->skeleton.y,
		actor->skeleton.w, actor->skeleton.h };
	// Handle each axis collision separately.
	if (delta->x != 0) // check x axis collision
	{
		if (!tilemap_collision(g_level, &actor_after, TILE_COLLISION)) // No collision, simple case.
		{
			actor->skeleton.x += delta->x;
			animation_set("move", &actor->anim);
		}
		else
		{
			while (true)
			{
				// Get as close as possible via stepping.
				// TODO: Is binary search feasible here?
				// No need, as long as I keep the speed <~100px/frame.
				if (delta->x > 0) // Moving right, --> collision from right.
					actor_after.x--;
				else // Collision from left.
					actor_after.x++;
				if (!tilemap_collision(g_level, &actor_after, TILE_COLLISION))
					break; // stop stepping
			}
			if (actor->skeleton.x == actor_after.x)
			{
				animation_set("move_blocked", &actor->anim);
			}
			else
			{
				actor->skeleton.x = actor_after.x;
				animation_set("move", &actor->anim);
			}
		}
	}
	actor_after.y += delta->y;
	if (delta->y != 0) // check y axis collision
	{
		if (!tilemap_collision(g_level, &actor_after, TILE_COLLISION))
		{
			// if not collide with ladder
			if(!tilemap_collision(g_level, &actor_after, LADDER_COLLISION))
				actor->state = AIR;
			actor->skeleton.y += delta->y;
		}
		else
		{
			while (true)
			{
				if (delta->y > 0)
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
					if (actor->velocity.y > FALLDAMAGE_TRESHHOLD)
						actor_damage(actor, ((int)actor->velocity.y % 8)*DAMAGE_RATE);
				}
			}
			actor->state = GROUND;
			actor->velocity.y = 0;
			actor->skeleton.y = actor_after.y;
		}
	}
	// Don't move legs when in air.
	if(actor->state == AIR)
		animation_set("move_blocked", &actor->anim);
	if (actor->velocity.x == 0)
		animation_set("stand", &actor->anim);
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

void actor_spawn(struct actor *actor)
{
	actor->skeleton.x = actor->spawn.x;
	actor->skeleton.y = actor->spawn.y;
	actor->is_visible = true;
	INFO("Actor %s spawned at [%d;%d].", actor->name, actor->spawn.x, actor->spawn.y);
}

void player_init(struct player *player, const char* name, const vec2 spawn, const char* anim_name, SDL_Renderer *renderer)
{
	actor_init(&player->actor, name, spawn, anim_name, renderer); 
	player->climb[0] = player->climb[1] = false;
	player->collect = 0;
}

void player_destroy(struct player *player)
{
	actor_destroy(&player->actor);
}

void player_draw(const struct player *player, SDL_Renderer *renderer)
{
	if (player->actor.is_visible)
	{
		SDL_Rect dest;
		dest.w = dest.h = 34; // Draw a bit larger.
		dest.x = player->actor.skeleton.x - g_camera.position.x;
		dest.y = player->actor.skeleton.y - g_camera.position.y;
		SDL_Rect* src = NULL;
		SDL_Texture* t = NULL;
		t = sprite_get(player->actor.anim.curr->curr->sprite_name, &src);
		if (player->actor.velocity.x < 0) // Moving left.
			SDL_RenderCopyEx(renderer, t, src, &dest, 0, 0, SDL_FLIP_HORIZONTAL);
		else
			SDL_RenderCopy(renderer, t, src, &dest);
		/*if (player->actor.state == LADDER)
		{
			
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
		*/
	}
}

void player_move(struct player *player, const vec2 *delta)
{
	actor_move(&player->actor, delta);
}

void player_jump(struct player *player, float speed) 
{ 
	// jump requirements
	if ((!player->actor.is_jumping && 
		player->actor.state == GROUND) 
		|| (player->actor.is_jumping && 
			player->actor.jump_count < MULTI_JUMP &&
			player->actor.state != LADDER))
	{
		// He is jumping until he hits the ground.
		player->actor.is_jumping = true;
		player->actor.jump_count++;
		// Go against gravity.
		player->actor.velocity.y = -speed;
		sound_play("jump");
	}
}

void player_spawn(struct player *player)
{
	actor_spawn(&player->actor);
}

bool player_can_climb(struct player *player)
{
	return tilemap_collision(g_level, &player->actor.skeleton, LADDER_COLLISION);
}

bool player_can_shoot(struct player* player)
{
	if (player->collect > 0 && player->actor.state != LADDER)
		return true;
	return false;
}

void player_climb(struct player *player)
{
	player->actor.state = LADDER;
	if (player->climb[0])
	{
		player_set_vel_y(player, PLAYER_CLIMB_SPEED);
		animation_set("ladder", &player->actor.anim);
	}
	else if (player->climb[1])
	{
		player_set_vel_y(player, -PLAYER_CLIMB_SPEED);
		animation_set("ladder", &player->actor.anim);
	}
	else
	{
		player_set_vel_y(player, 0);
		animation_set("ladder_calm", &player->actor.anim);
	}
}