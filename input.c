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
				game_pause(game);
				INFO("Game pause: %s", game->paused?"TRUE":"FALSE");
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

static void update_player_double_jump(struct actor *actor)
{
	/*
	* The two following functions should be called only if actor is PC --> maybe add a bool into the struct.
	*
	**/
	if (actor->is_jumping && actor->state == GROUND)
	{
		actor->is_jumping = false;
		actor->jump_count = 0;
	}
}

static void update_player_draw_state(struct actor *player)
{
	if (player->state != AIR)
	{
		if (player->draw_state < player->sprite_count - 1)
			player->draw_state += .2f;
		else
			player->draw_state = 0;
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
	update_player_double_jump(&g_player);
	update_player_draw_state(&g_player);
	// Update camera.
	camera_set(&g_camera, (vec2) { g_player.skeleton.x - CENTER_X, g_player.skeleton.y - CENTER_Y });
}

void update_edit(void)
{
	camera_scroll(&g_camera, (vec2) { (coord) g_player.velocity.x*g_player.skeleton.w, (coord) g_player.velocity.y*g_player.skeleton.h });
}