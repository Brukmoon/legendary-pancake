#include <SDL.h>
#include "game.h"
#include "camera.h"
#include "common.h"
#include "player.h"
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
				g_player.x_vel = 1*g_player.speed_coeff;
				break;
			case SDLK_LEFT:
				g_player.x_vel = -1*g_player.speed_coeff;
				break;
		    /*
			case SDLK_UP:
				player.y_vel = -1* PLAYER_SPEED;
				break;*/
			/*case SDLK_DOWN:
				player.y_vel = 1* PLAYER_SPEED;
				break;*/
			case SDLK_SPACE:
				if(g_player.state != FALL)
					g_player.y_vel = -10;
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
				g_player.x_vel = 0;
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
				g_player.x_vel = 1;
				break;
			case SDLK_LEFT:
				g_player.x_vel = -1;
				break;
			case SDLK_UP:
				g_player.y_vel = -1;
				break;
			case SDLK_DOWN:
				g_player.y_vel = 1;
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
				g_player.x_vel = 0;
				break;
			case SDLK_UP:
			case SDLK_DOWN:
				g_player.y_vel = 0;
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
	if (g_player.state == GROUND)
		g_player.y_vel += (float)(2*GRAVITY);
	if (g_player.y_vel <= T_VEL)
		g_player.y_vel += (float)GRAVITY;
	else
		g_player.y_vel = T_VEL;
	move_actor(&g_player, g_player.x_vel, g_player.y_vel);
}

void update_edit(void)
{
	scroll_camera(&g_camera, g_player.x_vel*g_player.skeleton.w, g_player.y_vel*g_player.skeleton.h);
}