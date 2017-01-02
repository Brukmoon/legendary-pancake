#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <stdbool.h>

#include "vector.h"

#define JUMP_COUNT 2

struct actor
{/*
#define ACTOR_NAME_LENGTH 20
	char name[ACTOR_NAME_LENGTH];*/
	// life
	int hitpoints;
	enum
	{
		GROUND,
		AIR
	} state;
	SDL_Texture *texture;
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
	vec2f velocity;
};
// TODO: Move to player struct.
extern struct actor g_player;

// Initialize actor.
// TODO: Add initialization options.
void actor_init(struct actor *actor, SDL_Renderer *renderer);
// Move actor by delta.
void actor_move(struct actor *actor, vec2 delta);
// Perform jump. Speed must be positive, not a vector.
void actor_jump(struct actor *actor, float speed);
// Subtract damage points from actor's HPs.
void actor_damage(struct actor *actor, int damage);
// Draw actor on screen.
void actor_draw(const struct actor *actor, SDL_Renderer *renderer);

#endif // PLAYER_H