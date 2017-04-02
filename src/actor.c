#include "actor.h"
#include "camera.h"
#include "collision.h"
#include "common.h"
#include "config.h"
#include "level.h"
#include "sound.h"
#include "physics.h"
#include "texture.h"
#include "game.h"

#define PLAYER_PATH IMG_PATH"player.png"
// How fast does a player climb a ladder.
#define ACTOR_STANDARD_SPEED 4.f
#define ACTOR_DEFAULT_HP 100
#define ACTOR_DEFAULT_X 0
#define ACTOR_DEFAULT_Y 0
//#define ACTOR_NAME_LENGTH 10
#define FALLDAMAGE_TRESHHOLD 10

struct player g_player;
// enemy linked list
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
static bool actor_can_shoot(struct actor const* actor);
// is on ladder tile?
static bool actor_can_climb(struct actor const* actor);
// subtract damage points from actor hitpoints
static void actor_damage(struct actor* actor, Uint16 const damage);
static void actor_gravity(struct actor* actor);
// put speed force on actor
static void actor_jump(struct actor* actor, float speed);

void actor_init(struct actor *actor, char const* name, vec2 const spawn, char const* anim_name, SDL_Renderer *renderer)
{
	size_t name_str_len = SDL_strlen(name) + 1;
	actor->name = malloc(name_str_len);
	SDL_strlcpy(actor->name, name, name_str_len);
	actor->hitpoints = ACTOR_DEFAULT_HP;
	actor->skeleton.x = ACTOR_DEFAULT_X;
	actor->skeleton.y = ACTOR_DEFAULT_Y;
	// size of character is tile size by default
	actor->skeleton.w = g_level->tile_map.tile_width;
	actor->skeleton.h = g_level->tile_map.tile_height;
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

static bool actor_can_climb(struct actor const* actor)
{
	return tilemap_collision(g_level, &actor->skeleton, LADDER_COLLISION);
}

void actor_destroy(struct actor *actor)
{
	free(actor->name);
	animation_table_destroy(&actor->anim);
}

void actor_draw(const struct actor *actor, int draw_size_delta, SDL_Renderer *renderer)
{
	if (actor->is_visible)
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
}

void actor_move(struct actor *actor, const vec2* delta)
{
	// Where is the actor after the movement?
	SDL_Rect actor_after = { actor->skeleton.x + delta->x, actor->skeleton.y,
		actor->skeleton.w, actor->skeleton.h };
	// Handle each axis collision separately.
	if (delta->x != 0) // check x axis collision
	{
		// no collision, move normally
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
			// collision, but no change
			if (actor->skeleton.x == actor_after.x)
				animation_set("move_blocked", &actor->anim);
			else
			{
				actor->skeleton.x = actor_after.x;
				animation_set("move", &actor->anim);
			}
		}
	}
	if (delta->y != 0) // check y axis collision
	{
		actor_after.y += delta->y;
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
					{
						// TODO: Move out.
						sound_play("fall");
						actor_damage(actor, ((int)actor->velocity.y % 8)*DAMAGE_RATE);
					}
				}
			}
			if(delta->y > 0)
				actor->state = GROUND;
			actor->velocity.y = 0;
			actor->skeleton.y = actor_after.y;
		}
	}
	if (delta->x == 0)
		animation_set("stand", &actor->anim);
	// Don't move legs when in air.
	else if(actor->state == AIR)
		animation_set("move_blocked", &actor->anim);
}

void actor_damage(struct actor *actor, Uint16 const damage)
{
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

static void actor_jump(struct actor* actor, float speed)
{
	if (actor->state != LADDER)
	{
		// He is jumping until he hits the ground.
		actor->is_jumping = true;
		actor->jump_count++;
		// Go against gravity.
		actor->velocity.y = -speed;
	}
}

void actor_gravity(struct actor* actor)
{
	if ((actor->velocity.y + (float)GRAVITY) <= T_VEL)
		actor->velocity.y += (float)GRAVITY;
	else
		actor->velocity.y = T_VEL;
}

void player_init(struct player *player, const char* name, const vec2 spawn, const char* anim_name, SDL_Renderer *renderer)
{
	actor_init(&player->actor, name, spawn, anim_name, renderer); 
	player->actor.hitpoints = PLAYER_DEFAULT_HP;
	player->climb[0] = player->climb[1] = false;
	player->collect = 0;
}

void player_destroy(struct player *player)
{
	actor_destroy(&player->actor);
}

void player_draw(const struct player *player, SDL_Renderer *renderer)
{
	actor_draw(&player->actor, 2, renderer);
}

void player_gravity(struct player* player)
{
	actor_gravity(&player->actor);
}

void player_move(struct player *player, const vec2 *delta)
{
	actor_move(&player->actor, delta);
}

void player_jump(struct player *player, float speed) 
{ 
	// jump requirements
	// TODO: ladder is checked twice
	if (
		(!player->actor.is_jumping && player->actor.state == GROUND) 
		|| 
		(player->actor.is_jumping && player->actor.jump_count < MULTI_JUMP && player->actor.state != LADDER)
		)
	{
		actor_jump(&player->actor, speed);
		sound_play("jump");
	}
}

void player_spawn(struct player *player)
{
	actor_spawn(&player->actor);
}

bool player_can_climb(const struct player *player)
{
	return actor_can_climb(&player->actor);
}

bool player_can_shoot(const struct player* player)
{
	if (player->collect > 0 && player->actor.state != LADDER && game_mode != MODE_PACIFIC)
		return true;
	return false;
}

void player_eat(struct player* player)
{
	// if there is collectable
	if (player->collect)
	{
		// player probably doesn't want to eat it
		if (player->actor.hitpoints == PLAYER_DEFAULT_HP)
			return;
		if ((player->actor.hitpoints + PLAYER_HEAL_RATE) <= PLAYER_DEFAULT_HP)
			player->actor.hitpoints += PLAYER_HEAL_RATE;
		else
			player->actor.hitpoints = PLAYER_DEFAULT_HP;
		sound_play("eat");
		player->collect--;
	}
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
	if (enemy->actor.is_visible)
	{
		actor_draw(&enemy->actor, 0, renderer);
	}
}

void enemy_load(char const* name, char const* anim_name, vec2 const spawn, vec2 const goal, enum enemy_type t, SDL_Renderer *renderer)
{
	INFO("Enemy load %s, SPAWN: [%d; %d] GOAL: [%d; %d]", name ,spawn.x, spawn.y, goal.x, goal.y);
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
	enemy->type = t;
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
		iter->next = enemy;
	}
}

void enemy_draw_all(SDL_Renderer* renderer)
{
	struct enemy* iter = g_enemies;
	SDL_Rect dest = { 0, 0 };
	while (iter)
	{
		vec2 enemy_pos = { iter->actor.skeleton.x, iter->actor.skeleton.y };
		if (iter->is_spawned && is_visible(&g_camera, &enemy_pos, iter->actor.skeleton.w, iter->actor.skeleton.h))
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
		}
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
		// not spawned
		if (!iter->is_spawned)
		{
			// process the next one
			iter = iter->next;
			continue;
		}
		// check if dead
		// TODO: Add dead texture -> should be an object.
		if (iter->actor.hitpoints <= 0)
		{
			iter->is_spawned = false; // despawn
			iter = iter->next;
			continue;
		}
		// check collision with player
		if (rects_collide(&g_player.actor.skeleton, &iter->actor.skeleton))
		{
			player_damage(&g_player, (const Uint16)(DAMAGE_RATE*0.1));
			// skip move
			iter = iter->next;
			continue;
		}
		// handle gravity
		actor_gravity(&iter->actor);
		vec2 enemy_pos = { 0, 0 }, enemy_pos_real = { 0, 0 };
		// should jump
		bool jumping = false;
		enemy_pos_real.x = iter->actor.skeleton.x;
		enemy_pos_real.y = iter->actor.skeleton.y;
		enemy_pos = real_to_map(iter->actor.skeleton.x, iter->actor.skeleton.y);
		if (iter->path) // there is a path
		{	
			// there was a previous waypoint and it was to the right
			if (iter->current->prev && iter->current->prev->pos.x > iter->current->pos.x)
				enemy_pos.x++;
			if (iter->current->prev && iter->current->prev->pos.y > iter->current->pos.y)
				enemy_pos.y++;
			vec2 start_real = vec2_scale(&iter->start, g_level->tile_map.tile_width);
			// we reached the end point
			if (vec2_similar(&enemy_pos_real, &start_real, 2))
			{
				vec2_swap(&iter->start, &iter->goal);
				path_destroy(&iter->path);
				iter->current = NULL;
				// attempt to find a path
				path_find(iter->start, iter->goal, &iter->path);
				iter->current = iter->path;
				if (!iter->current)
				{
					INFO("Path not found.");
					iter = iter->next;
					continue;
				}
			}
			vec2 waypoint_real = vec2_scale(&iter->current->pos, g_level->tile_map.tile_width);
			if (vec2_similar(&waypoint_real, &enemy_pos_real, 3))
			{
				if(iter->current->next)
					iter->current = iter->current->next; // set next
			}
			// is there a path?
			if (iter->current)
			{
				// must go up
				if (iter->current->pos.y < enemy_pos.y)
				{
					iter->actor.velocity.y = -2;
					jumping = true;
				}
				else if (iter->current->pos.y > enemy_pos.y)
				{
					if (actor_can_climb(&iter->actor))
						iter->actor.velocity.y = 2;
				}
				if (iter->current->pos.x < enemy_pos.x)
					iter->actor.velocity.x = -2;
				else if (iter->current->pos.x > enemy_pos.x)
					iter->actor.velocity.x = 2;
				else
					iter->actor.velocity.x = 0;
			}
		}
		if (jumping)
		{
			// stuck, big jump
			if((abs(iter->current->pos.y-iter->actor.skeleton.y/32) > 1) || (iter->current->pos.y != (iter->actor.skeleton.y / 32) && iter->current->pos.x != (iter->actor.skeleton.x/32)))
				actor_jump(&iter->actor, 5.5f);
			// normal jump
			else
				actor_jump(&iter->actor, 4.5f);

		}
		vec2 move_delta = { (coord) iter->actor.velocity.x, (coord) iter->actor.velocity.y };
		actor_move(&iter->actor, &move_delta);
		if (tilemap_collision(g_level, &iter->actor.skeleton, LADDER_COLLISION))
		{
			iter->actor.state = LADDER;
			animation_set("ladder", &iter->actor.anim);
		}
		// animation
		iter->actor.anim.curr->delay_counter += 1000 / FPS;
		if (iter->actor.anim.curr->delay_counter > iter->actor.anim.curr->delay)
		{
			animation_next(&iter->actor.anim);
			iter->actor.anim.curr->delay_counter = 0;
		}
		iter = iter->next;
	}
}

void enemy_write_to_file(FILE* f)
{
	struct enemy* iter = g_enemies;
	unsigned k = g_level->tile_map.tile_width;
	while (iter)
	{
		fprintf(f, "ENEMY enemy patrol %d %d %d %d\n", k*iter->start.x, k*iter->start.y, k*iter->goal.x, k*iter->goal.y);
		iter = iter->next;
	}
}

void enemy_spawn(struct enemy* enemy)
{
	path_find(enemy->start, enemy->goal, &enemy->path);
	if (enemy->actor.skeleton.y == 627)
		INFO("%d %d", enemy->path->pos.y, enemy->path->pos.x);
	if (!enemy->path)
	{
		INFO("Path not found.");
	}
	enemy->current = enemy->path;
	enemy->is_spawned = true;
}