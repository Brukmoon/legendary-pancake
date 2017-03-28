/*
 * What is an actor? A fancy name for anything that moves, e.g. an enemy or the player.
 * TODO: Add an animation structure to the actor.
 *
 * @author Michal H.
 *
 */
#ifndef ACTOR_H
#define ACTOR_H

#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>

#include "animation.h"
#include "path.h"

// Beware: If you change MULTI_JUMP, modify the UI (stamina) aswell.
enum 
{ 
	MULTI_JUMP = 2 
};

// How much does a player jump.
#define PLAYER_JUMP_INTENSITY 6.f  

struct actor
{
	char* name;
	enum
	{
		GROUND,
		AIR,
		LADDER
	} state;

	// sizes and position
	SDL_Rect skeleton;
	// Where should the actor spawn?
	vec2 spawn;
	// speed coefficient
	float speed;

	struct animation_table anim;

	bool is_jumping;
	// should the actor be drawn?
	bool is_visible;
	// jump count since last reset
	Uint8 jump_count;
	// current velocity (horizontal and vertical)
	vec2f velocity;
	// life; max 32,767 HPs.
	Sint16 hitpoints;
};

// special case of an actor.
struct player // : public actor;
{
	struct actor actor;
	// climb[0] -> down, climb[1] -> up
	bool climb[2];
	// how many collectables has he collected?
	// TODO: Move into backpack struct.
	Uint8 collect;
};

// @anim_name: which animation table file to load?
void player_init(struct player* player, char const* name, vec2 const spawn, char const* anim_name, SDL_Renderer* renderer);
void player_destroy(struct player* player);

void player_draw(const struct player* player, SDL_Renderer* renderer);
void player_gravity(struct player* player);
void player_move(struct player* player, const vec2* delta);
void player_jump(struct player* player, float speed);
// place the player at spawn position and spawn him
void player_spawn(struct player* player);
// is the player currently on ladder?
bool player_can_climb(const struct player* player);
bool player_can_shoot(const struct player* player);
void player_climb(struct player* player);
// Cause damage to the player.
void player_damage(struct player *player, Uint16 const damage);

inline void player_set_spawn(struct player *player, vec2 const spawn) { player->actor.spawn = spawn; }
// Set player velocity to vel.
inline void player_set_vel_x(struct player *player, float vel) { player->actor.velocity.x = vel; }
inline void player_set_vel_y(struct player *player, float vel) { player->actor.velocity.y = vel; }

extern struct player g_player;

enum enemy_type
{
	PATROL,
	RAIDER
};

struct enemy
{
	struct actor actor;
	enum enemy_type type;

	vec2 start, goal;
	struct waypoint* path, *current;
	bool is_spawned;

	struct enemy* next;
};

void enemy_init(struct enemy* enemy, char const* name, vec2 const spawn, char const* anim_name, SDL_Renderer* renderer);
static void enemy_destroy(struct enemy* enemy);

// destroy all enemies (@ level cleanup)
void enemy_destroy_all(void);
void enemy_draw_all(SDL_Renderer* renderer);
void enemy_update_all(void);

void enemy_draw(struct player const* enemy, SDL_Renderer* renderer);
void enemy_load(char const* name, char const* anim_name, vec2 const spawn, vec2 const goal, enum enemy_type t, SDL_Renderer *renderer);
void enemy_spawn(struct enemy* enemy);

// write to level file
void enemy_write_to_file(FILE* f);

extern struct enemy* g_enemies;

#endif // ACTOR_H