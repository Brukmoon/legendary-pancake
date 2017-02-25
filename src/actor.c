#include "actor.h"
#include "camera.h"
#include "collision.h"
#include "common.h"
#include "config.h"
#include "level.h"
#include "sound.h"
#include "physics.h"
#include "texture.h"
#include "vector.h"

#define PLAYER_PATH IMG_PATH"player.png"

struct player g_player;
struct enemy* g_enemies = NULL;

// Initialize the actor object.
static void actor_init(struct actor* actor, char const* name, vec2 const spawn, char const* anim_name, SDL_Renderer* renderer);
static void actor_destroy(struct actor* actor);

// Spawn actor. Call actor_init first.
static void actor_spawn(struct actor* actor);
// Move actor by delta.
static void actor_move(struct actor* actor, vec2 const* delta);
// Subtract damage points from actor's HPs.
static void actor_draw(struct actor const* actor, int draw_size_delta, SDL_Renderer* renderer);
static bool actor_can_shoot(const struct actor* actor);
static void actor_damage(struct actor* actor, Uint16 const damage);


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

// NYI
bool actor_can_shoot(const struct actor* actor)
{
	return true;
}

void actor_destroy(struct actor *actor)
{
	free(actor->name);
	animation_table_destroy(&actor->anim);
}

// NYI
void actor_draw(const struct actor *actor, int draw_size_delta, SDL_Renderer *renderer)
{
	SDL_Rect dest;
	dest.w = dest.h = 32 + draw_size_delta; // Draw a bit larger.
	dest.x = actor->skeleton.x - g_camera.position.x;
	dest.y = actor->skeleton.y - g_camera.position.y;
	SDL_Rect* src = NULL;
	SDL_Texture* t = NULL;
	t = sprite_get(actor->anim.curr->curr->sprite_name, &src);
	if (actor->velocity.x < 0) // Moving left.
		SDL_RenderCopyEx(renderer, t, src, &dest, 0, 0, SDL_FLIP_HORIZONTAL);
	else
		SDL_RenderCopy(renderer, t, src, &dest);
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
	if (delta->x == 0)
		animation_set("stand", &actor->anim);
}

void actor_damage(struct actor *actor, Uint16 const damage)
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
	player->path = NULL;
}

void player_destroy(struct player *player)
{
	actor_destroy(&player->actor);
	path_destroy(&player->path);
}

void player_draw(const struct player *player, SDL_Renderer *renderer)
{
	if (player->actor.is_visible)
	{
		actor_draw(&player->actor, 2, renderer);
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

bool player_can_climb(const struct player *player)
{
	return tilemap_collision(g_level, &player->actor.skeleton, LADDER_COLLISION);
}

bool player_can_shoot(const struct player* player)
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

void player_damage(struct player *player, Uint16 const damage)
{ 
	actor_damage(&player->actor, damage); 
}

void enemy_init(struct enemy* enemy, char const* name, vec2 const spawn, char const* anim_name, SDL_Renderer* renderer)
{
	actor_init(&enemy->actor, name, spawn, anim_name, renderer);
	enemy->is_spawned = false;
	enemy->current = NULL;
	enemy->path = NULL;
	enemy->next = NULL;
}

void enemy_destroy(struct enemy* enemy)
{
	actor_destroy(&enemy->actor);
	path_destroy(&enemy->path);
}

void enemy_draw(struct player const* enemy, SDL_Renderer* renderer)
{
	// TODO: Move into actor_draw.
	if (enemy->actor.is_visible)
	{
		actor_draw(&enemy->actor, 0, renderer);
	}
}

void enemy_load(char const* name, char const* anim_name, vec2 const spawn, vec2 const goal, SDL_Renderer *renderer)
{
	INFO("ENEMY LOAD %d %d GOAL: %d %d", spawn.x, spawn.y, goal.x, goal.y);
	struct enemy* enemy = malloc(sizeof(struct enemy));
	if (!enemy)
	{
		ERROR("Not enough memory!");
		return;
	}
	enemy_init(enemy, name, spawn, anim_name, renderer);
	enemy->actor.skeleton.x = enemy->actor.spawn.x;
	enemy->actor.skeleton.y = enemy->actor.spawn.y;
	enemy->start = real_to_map(spawn.x, spawn.y);
	enemy->goal = real_to_map(goal.x, goal.y);
	enemy_spawn(enemy);

	// empty list
	if (!g_enemies)
	{
		g_enemies = enemy;
	}
	// push at end
	else
	{
		struct enemy* iter = g_enemies;
		while (iter->next)
		{
			iter = iter->next;
		}
		iter->next = iter;
	}
}

void enemy_draw_all(SDL_Renderer* renderer)
{
	struct enemy* iter = g_enemies;
	SDL_Rect dest;
	while (iter)
	{
		dest.w = iter->actor.skeleton.w;
		dest.h = iter->actor.skeleton.h;
		dest.x = iter->actor.skeleton.x - g_camera.position.x;
		dest.y = iter->actor.skeleton.y - g_camera.position.y;
		SDL_Rect* src = NULL;
		SDL_Texture* t = NULL;
		t = sprite_get(iter->actor.anim.curr->curr->sprite_name, &src);
		if (iter->actor.velocity.x < 0) // Moving left.
			SDL_RenderCopyEx(renderer, t, src, &dest, 0, 0, SDL_FLIP_HORIZONTAL);
		else
			SDL_RenderCopy(renderer, t, src, &dest);
		iter = iter->next;
	}
}

void enemy_destroy_all(void)
{
	struct enemy* iter = g_enemies, *prev = iter;
	while (iter)
	{
		iter = iter->next;
		enemy_destroy(prev);
		free(prev);
		prev = iter;
	}
	g_enemies = NULL;
}

void enemy_update_all(void)
{
	struct enemy* iter = g_enemies;
	while (iter)
	{
		if ((iter->actor.velocity.y + (float)GRAVITY) <= T_VEL) // Player can't exceed terminal velocity.
			iter->actor.velocity.y += (float)GRAVITY;
		else
			iter->actor.velocity.y = T_VEL;
		vec2 enemy_pos = { 0, 0 };

		if (iter->current) // there is a path
		{
			enemy_pos = real_to_map(iter->actor.skeleton.x, iter->actor.skeleton.y);
			if (iter->current->prev && iter->current->prev->pos.x > iter->current->pos.x)
				enemy_pos.x++;
			if (iter->current->prev && iter->current->prev->pos.y > iter->current->pos.y)
				enemy_pos.y++;
			if (vec2_equal(&enemy_pos, &iter->start))
			{
				vec2_swap(&iter->start, &iter->goal);
				path_destroy(&iter->path);
				path_find(iter->start, iter->goal, &iter->path);
				iter->current = iter->path;
			}
			// reached current waypoint
			if (vec2_equal(&iter->current->pos, &enemy_pos))
				iter->current = iter->current->next; // set next
			// must go up
			if (iter->current->pos.y < enemy_pos.y)
				iter->actor.velocity.y = -2;
			else if (iter->current->pos.y > enemy_pos.y)
				iter->actor.velocity.y = 2;
			if (iter->current->pos.x < enemy_pos.x)
				iter->actor.velocity.x = -2;
			else if (iter->current->pos.x > enemy_pos.x)
				iter->actor.velocity.x = 2;
			else
				iter->actor.velocity.x = 0;
		}
		vec2 move_speed = { iter->actor.velocity.x, iter->actor.velocity.y };
		actor_move(&iter->actor, &move_speed);
		iter->actor.anim.curr->delay_counter += 1000 / FPS;
		if (iter->actor.anim.curr->delay_counter > iter->actor.anim.curr->delay)
		{
			animation_next(&iter->actor.anim);
			iter->actor.anim.curr->delay_counter = 0;
		}
		iter = iter->next;
	}
}

void enemy_spawn(struct enemy* enemy)
{
	path_find(enemy->start, enemy->goal, &enemy->path);
	enemy->current = enemy->path;
	enemy->is_spawned = true;
}