#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include "position.h"

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
	float speed_coeff;
	float x_vel, y_vel;
	int hp;
};
extern struct actor g_player;

void init_actor(struct actor *actor, SDL_Renderer *renderer);
void move_actor(struct actor *actor, float d_x, float d_y);
void draw_actor(const struct actor *actor, SDL_Renderer *renderer);

#endif // PLAYER_H