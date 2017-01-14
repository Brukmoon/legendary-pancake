#include <SDL.h>

#include "actor.h"
#include "game.h"
#include "camera.h"
#include "common.h"
#include "collision.h"
#include "menu.h"
#include "sound.h"
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
			case SDLK_UP:
				menu_prev_button(g_menu);
				break;
			case SDLK_DOWN:
				menu_next_button(g_menu);
				break;
			case SDLK_RETURN:
				sound_play("accept");
				if (SDL_strcmp(M_MENU_PLAY, g_menu->button_list.current->text) == 0)
					game_set_state(game, PLAY);
				else if (SDL_strcmp(M_MENU_EDIT, g_menu->button_list.current->text) == 0)
					game_set_state(game, EDIT);
				else if(SDL_strcmp(M_MENU_QUIT, g_menu->button_list.current->text) == 0)
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
				player_set_vel_x(&g_player, g_player.actor.speed);
				break;
			case SDLK_LEFT:
				player_set_vel_x(&g_player, -g_player.actor.speed);
				break;
		    /*
			case SDLK_UP:
				player.velocity.y = -1* PLAYER_SPEED;
				break;*/
			/*case SDLK_DOWN:
				player.velocity.y = 1* PLAYER_SPEED;
				break;*/
			case SDLK_RETURN:
				game_pause(game);
				INFO("Game pause: %s", game->paused?"TRUE":"FALSE");
				break;
			case SDLK_SPACE:
				actor_jump(&g_player.actor, 7.f);
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
				player_set_vel_x(&g_player, 0);
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
				player_set_vel_x(&g_player, g_player.actor.speed);
				break;
			case SDLK_LEFT:
				player_set_vel_x(&g_player, -g_player.actor.speed);
				break;
			case SDLK_UP:
				player_set_vel_y(&g_player, -g_player.actor.speed);
				break;
			case SDLK_DOWN:
				player_set_vel_y(&g_player, g_player.actor.speed);
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
				player_set_vel_x(&g_player, 0);
				break;
			case SDLK_UP:
			case SDLK_DOWN:
				player_set_vel_y(&g_player, 0);
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

static void update_player_double_jump(struct player *player)
{
	/*
	* The two following functions should be called only if actor is PC --> maybe add a bool into the struct.
	*
	**/
	if (player->actor.is_jumping && player->actor.state == GROUND)
	{
		player->actor.is_jumping = false;
		player->actor.jump_count = 0;
	}
}

static void update_player_draw_state(struct player *player)
{
	if (player->actor.state != AIR)
	{
		if (player->actor.draw_state < player->actor.sprite_count - 1)
			player->actor.draw_state += .2f;
		else
			player->actor.draw_state = 0;
	}
}

void update_menu(void)
{
}

void update_play(void)
{
	// Update player.
	if ((g_player.actor.velocity.y + (float)GRAVITY) <= T_VEL) // Player can't exceed terminal velocity.
		g_player.actor.velocity.y += (float)GRAVITY;
	else
		player_set_vel_y(&g_player, T_VEL);
	// Move him.
	player_move(&g_player, (vec2) { (coord) g_player.actor.velocity.x, (coord) g_player.actor.velocity.y });
	update_player_double_jump(&g_player);
	update_player_draw_state(&g_player);
	// Update camera.
	camera_set(&g_camera, (vec2) { g_player.actor.skeleton.x - CENTER_X, g_player.actor.skeleton.y - CENTER_Y });
}

void update_edit(void)
{
	camera_scroll(&g_camera, (vec2) { (coord) g_player.actor.velocity.x*g_player.actor.skeleton.w, (coord) g_player.actor.velocity.y*g_player.actor.skeleton.h });
}