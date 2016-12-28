#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include "vector.h"

struct actor
{
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
	// speed coefficient
	float speed;
	vec2f velocity;
	int hp;
};
extern struct actor g_player;

void init_actor(struct actor *actor, SDL_Renderer *renderer);
void move_actor(struct actor *actor, vec2 delta);
void jump_actor(struct actor *actor, float speed);
void draw_actor(const struct actor *actor, SDL_Renderer *renderer);

#endif // PLAYER_H