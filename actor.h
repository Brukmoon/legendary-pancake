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
extern struct actor g_player;

void actor_init(struct actor *actor, SDL_Renderer *renderer);
void actor_move(struct actor *actor, vec2 delta);
void actor_jump(struct actor *actor, float speed);
void actor_damage(struct actor *actor, int damage);
void actor_draw(const struct actor *actor, SDL_Renderer *renderer);

#endif // PLAYER_H