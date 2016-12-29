#include <SDL.h>

#include "actor.h"
#include "game.h"
#include "camera.h"
#include "common.h"
#include "collision.h"
#include "input.h"
#include "physics.h"

void process_input_menu(struct game* game)
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) 
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) 
			{
			case SDLK_RIGHT:
				break;
			case SDLK_LEFT:
				break;
			case SDLK_UP:
				break;
			case SDLK_DOWN:
				break;
			case SDLK_e:
				game_set_state(game, EDIT);
				break;
			case SDLK_p:
				game_set_state(game, PLAY);
				break;
			case SDLK_q:
				game_set_state(game, EXIT);
				break;
			default:
				break;
			}
			break;
		case SDL_QUIT:
			game_set_state(game, EXIT);
			INFO("EXIT flag set.");
			break;
		}
	}
}

void process_input_play(struct game* game)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_RIGHT:
				g_player.velocity.x = 1*g_player.speed;
				break;
			case SDLK_LEFT:
				g_player.velocity.x = -1*g_player.speed;
				break;
		    /*
			case SDLK_UP:
				player.velocity.y = -1* PLAYER_SPEED;
				break;*/
			/*case SDLK_DOWN:
				player.velocity.y = 1* PLAYER_SPEED;
				break;*/
			case SDLK_RETURN:
				game_set_pause(game, !game->paused);
				INFO("Game pause:%d.", game->paused);
				break;
			case SDLK_SPACE:
				actor_jump(&g_player, 7.f);
				break;
			case SDLK_q:
				game_set_state(game, EXIT);
				break;
			case SDLK_m:
				game_set_state(game, MENU);
				break;
			case SDLK_p:
				game_pause(game);
				break;
			default:
				break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case SDLK_RIGHT:
			case SDLK_LEFT:
				g_player.velocity.x = 0;
				break;
			case SDLK_q:
				game_set_state(game, EXIT);
				break;
			case SDLK_m:
				game_set_state(game, MENU);
				break;
			default:
				break;
			}
			break;
		case SDL_QUIT:
			game_set_state(game, EXIT);
			INFO("EXIT flag set.");
			break;
		}
	}
}

void process_input_edit(struct game *game)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_RIGHT:
				g_player.velocity.x = 1;
				break;
			case SDLK_LEFT:
				g_player.velocity.x = -1;
				break;
			case SDLK_UP:
				g_player.velocity.y = -1;
				break;
			case SDLK_DOWN:
				g_player.velocity.y = 1;
				break;
			case SDLK_q:
				game_set_state(game, EXIT);
				break;
			case SDLK_m:
				game_set_state(game, MENU);
				break;
			default:
				break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case SDLK_RIGHT:
			case SDLK_LEFT:
				g_player.velocity.x = 0;
				break;
			case SDLK_UP:
			case SDLK_DOWN:
				g_player.velocity.y = 0;
				break;
			case SDLK_q:
				game_set_state(game, EXIT);
				break;
			case SDLK_m:
				game_set_state(game, MENU);
				break;
			default:
				break;
			}
			break;
		case SDL_MOUSEMOTION:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			break;
		case SDL_QUIT:
			game_set_state(game, EXIT);
			INFO("EXIT flag set.");
			break;
		}
	}
}

void update_menu(void)
{
}

void update_play(void)
{
	// Update player.
	if ((g_player.velocity.y + (float)GRAVITY) <= T_VEL) // Player can't exceed terminal velocity.
		g_player.velocity.y += (float)GRAVITY;
	else
		g_player.velocity.y = T_VEL;
	// Move him.
	actor_move(&g_player, (vec2) { (coord) g_player.velocity.x, (coord) g_player.velocity.y });
}

void update_edit(void)
{
	camera_scroll(&g_camera, (vec2) { (coord) g_player.velocity.x*g_player.skeleton.w, (coord) g_player.velocity.y*g_player.skeleton.h });
}