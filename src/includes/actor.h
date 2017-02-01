/*
 * What is an actor? A fancy name for anything that moves, e.g. an enemy or the player.
 * TODO: Add an animation structure to the actor.
 *
 * @author Michal H.
 *
 */
#ifndef ACTOR_H
#define ACTOR_H

 // How much does a player jump.
#define PLAYER_JUMP_INTENSITY 6.f
#define PLAYER_CLIMB_SPEED 5

#define ACTOR_STANDARD_SPEED 4.f
#define ACTOR_HP 100

#define FALLDAMAGE_TRESHHOLD 10
#define DAMAGE_RATE 20

#include <SDL.h>
#include <stdbool.h>

#include "animation.h"
#include "vector.h"

// Beware: If you change MULTI_JUMP, modify the UI (stamina) aswell.
enum 
{ 
	MULTI_JUMP = 2 
};

struct actor
{
	enum
	{
		GROUND,
		AIR,
		LADDER
	} state;

	// where?
	SDL_Rect skeleton;
	// Name of the actor.
	char *name;
	// Where should the actor spawn?
	vec2 spawn;
	// speed coefficient
	float speed;

	struct animation_table anim;

	// State of the actor.
	bool is_jumping;
	// Should be drawn?
	bool is_visible;
	Uint8 jump_count;
	// current velocity
	vec2f velocity;
	// Life: max 32,767 HPs.
	Sint16 hitpoints;
};

// Initialize actor.
void actor_init(struct actor *actor, const char* name, const vec2 spawn, const char* anim_name, SDL_Renderer *renderer);
void actor_destroy(struct actor *actor);

// Spawn actor. Call actor_init first.
void actor_spawn(struct actor *actor);
// Move actor by delta.
void actor_move(struct actor *actor, const vec2* delta);
// Subtract damage points from actor's HPs.
void actor_damage(struct actor *actor, const Sint16 damage);
void actor_draw(const struct actor *actor, SDL_Renderer *renderer);

// Player is a special case of an actor.
struct player // : public actor;
{
	// A player is an actor.
	struct actor actor;
	// climb[0] -> down, climb[1] -> up
	bool climb[2];
	int collect;
};

void player_init(struct player *player, const char* name, const vec2 spawn, const char* anim_name, SDL_Renderer *renderer);
void player_destroy(struct player *player);

void player_draw(const struct player *player, SDL_Renderer *renderer);
void player_move(struct player *player, const vec2* delta);
void player_jump(struct player *player, float speed);
// place the player at spawn position and spawn him
void player_spawn(struct player *player);
// is the player currently on ladder?
bool player_can_climb(struct player *player);
bool player_can_shoot(struct player* player);
void player_climb(struct player *player);

// Cause damage to the player.
inline void player_damage(struct player *player, const Sint16 damage) { actor_damage(&player->actor, damage); }

inline void player_set_spawn(struct player *player, const vec2 spawn) { player->actor.spawn = spawn; }
// Set player velocity to vel.
inline void player_set_vel_x(struct player *player, float vel) { player->actor.velocity.x = vel; }
inline void player_set_vel_y(struct player *player, float vel) { player->actor.velocity.y = vel; }

extern struct player g_player;

#endif // ACTOR_H