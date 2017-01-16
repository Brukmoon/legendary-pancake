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
#include <stdbool.h>

#include "vector.h"

// Size of the name storage.
#define ACTOR_NAME_MAX_LENGTH 20

// Beware: If you change JUMP_COUNT, modify the UI aswell.
enum 
{ 
	JUMP_COUNT = 2 
};

struct actor
{
	// Name of the actor.
	char name[ACTOR_NAME_MAX_LENGTH];
	// Life: max 32,767 HPs.
	Sint16 hitpoints;
	// State of the actor.
	enum
	{
		GROUND,
		AIR
	} state;
	bool is_jumping;
	// TODO: Move to an animation struct.
	// Current state of the animation.
	float draw_state;
	// Number of sprites tied to the actor
	int sprite_count;

	vec2 spawn;
	struct
	{
		int x, y, w, h;
	} skeleton;

	unsigned short jump_count;

	// speed coefficient
	float speed;
	// current velocity
	vec2f velocity;
};

// Initialize actor.
// TODO: Add initialization options.
void actor_init(struct actor *actor);
// Move actor by delta.
void actor_move(struct actor *actor, const vec2 delta);
// Perform jump. Speed must be positive, not a vector.
bool actor_jump(struct actor *actor, float speed);
// Subtract damage points from actor's HPs.
void actor_damage(struct actor *actor, const Sint16 damage);
// Draw actor on screen.
void actor_draw(const struct actor *actor, SDL_Renderer *renderer);

// Player is a special case of actor.
struct player // : public actor;
{
	// A player is an actor.
	struct actor actor;
	// Texture will be tied to him.
	SDL_Texture *texture;
};

// Initialize player.
void player_init(struct player *player, const vec2 spawn, SDL_Renderer *renderer);
void player_draw(const struct player *player, SDL_Renderer *renderer);
void player_move(struct player *player, const vec2 delta);
void player_jump(struct player *player, float speed);
// Cause damage to player.
inline void player_damage(struct player *player, const Sint16 damage) { actor_damage(&player->actor, damage); }
// Set player velocity to vel.
inline void player_set_vel_x(struct player *player, float vel) { player->actor.velocity.x = vel; }
inline void player_set_vel_y(struct player *player, float vel) { player->actor.velocity.y = vel; }

// TODO: Move to player struct.
extern struct player g_player;

#endif // ACTOR_H