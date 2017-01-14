#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <stdbool.h>

#include "vector.h"

// If JUMP_COUNT changes, modify the UI aswell.
#define JUMP_COUNT 2

struct actor
{
#define ACTOR_NAME_LENGTH 20
	char name[ACTOR_NAME_LENGTH];
	// life
	int hitpoints;
	enum
	{
		GROUND,
		AIR
	} state;
	float draw_state;
	int sprite_count;
	struct
	{
		int x, y, w, h;
	} skeleton;

	bool is_jumping;
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
void actor_move(struct actor *actor, vec2 delta);
// Perform jump. Speed must be positive, not a vector.
void actor_jump(struct actor *actor, float speed);
// Subtract damage points from actor's HPs.
void actor_damage(struct actor *actor, int damage);
// Draw actor on screen.
void actor_draw(const struct actor *actor, SDL_Renderer *renderer);

// Player is a special case of actor.
struct player // : public actor;
{
	// A player is an actor.
	struct actor actor;
	SDL_Texture *texture;
};

void player_init(struct player *player, SDL_Renderer *renderer);
void player_draw(const struct player *player, SDL_Renderer *renderer);
void player_move(const struct player *player, const vec2 delta);
inline void player_jump(struct player *player, float speed) { actor_jump(&player->actor, speed); }
inline void player_damage(struct player *player, int damage) { actor_damage(&player->actor, damage); }
inline void player_set_vel_x(struct player *player, float vel) { player->actor.velocity.x = vel; }
inline void player_set_vel_y(struct player *player, float vel) { player->actor.velocity.y = vel; }

// TODO: Move to player struct.
extern struct player g_player;


#endif // PLAYER_H