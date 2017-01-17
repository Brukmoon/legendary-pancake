#include <SDL.h>

#include "actor.h"
#include "game.h"
#include "camera.h"
#include "common.h"
#include "level.h"
#include "menu.h"
#include "sound.h"
#include "input.h"
#include "physics.h"

int curr_sprite_num = 1;

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
				// TODO: Redesign - send accept message to menu.
				sound_play("accept");
				if (SDL_strcmp(M_MENU_PLAY, g_menu->button_list.current->text) == 0)
				{
					level_load(1, game->screen.renderer); // load level 1
					game_set_state(game, PLAY);
					player_init(&g_player, game->screen.renderer);
					player_spawn(&g_player);
				}
				else if (SDL_strcmp(M_MENU_EDIT, g_menu->button_list.current->text) == 0)
				{
					game_set_state(game, EDIT);
					player_init(&g_player, game->screen.renderer);
					player_spawn(&g_player);
				}
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
	if (!game->paused)
	{
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
					break;
				case SDLK_SPACE:
					player_jump(&g_player, 7.f);
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
	else
		while (SDL_PollEvent(&event))
		{
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN)
				game_pause(game);
		}

}

static void write_map_texture(const vec2 *position, int value)
{
	INFO("Setting map tile texture at position [%d;%d] to value %d!", position->x, position->y, value);
	g_level->tile_map.map[TMAP_TEXTURE_LAYER][position->y][position->x] = value;
}
static void toggle_map_tile_coll(const vec2 *position)
{
	bool t = !g_level->tile_map.map[TMAP_COLLISION_LAYER][position->y][position->x];
	INFO("Writing map tile collision data at position [%d;%d] to %s!", position->x, position->y, t?"TRUE":"FALSE");
	g_level->tile_map.map[TMAP_COLLISION_LAYER][position->y][position->x] = t;
}

static const vec2 calc_mouse_pos_map()
{
	vec2 position = { 0, 0 };
	SDL_GetMouseState(&position.x, &position.y);
	position.x = (int)(g_camera.position.x + position.x) / g_level->tile_map.tile_width;
	position.y = (int)(g_camera.position.y + position.y) / g_level->tile_map.tile_height;
	if (position.x >= g_level->tile_map.width)
		position.x = g_level->tile_map.width - 1;
	else if (position.x < 0)
		position.x = 0;
	if (position.y >= g_level->tile_map.height)
		position.y = g_level->tile_map.height - 1;
	else if (position.y < 0)
		position.y = 0;
	return position;
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
				player_set_vel_x(&g_player, 1);
				break;
			case SDLK_LEFT:
				player_set_vel_x(&g_player, -1);
				break;
			case SDLK_UP:
				player_set_vel_y(&g_player, -1);
				break;
			case SDLK_DOWN:
				player_set_vel_y(&g_player, 1);
				break;
			case SDLK_p:
				game_set_state(game, PLAY);
				break;
			case SDLK_s:
				level_save();
				break;
			case SDLK_x:
			{
				vec2 position = { 0, 0 };
				SDL_GetMouseState(&position.x, &position.y);
				position.x = position.x + g_camera.position.x - g_player.actor.skeleton.w/2;
				position.y = position.y + g_camera.position.y - g_player.actor.skeleton.h/2;
				player_set_spawn(&g_player, position);
			}
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
		case SDL_MOUSEWHEEL:
			if (event.wheel.y > 0)
				curr_sprite_num += 1;
			else if (event.wheel.y < 0)
				curr_sprite_num -= 1;
			INFO("Current sprite changed to %d.", curr_sprite_num);
			break;
		case SDL_MOUSEBUTTONDOWN:
			sound_play("click");
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
			{
				vec2 position = calc_mouse_pos_map();
				write_map_texture(&position, curr_sprite_num);
				break;
			}
			case SDL_BUTTON_RIGHT:
			{
				vec2 position = calc_mouse_pos_map();
				write_map_texture(&position, 0);
				break;
			}
			case SDL_BUTTON_MIDDLE:
			{
				vec2 position = calc_mouse_pos_map();
				toggle_map_tile_coll(&position);
			}
			}
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

void update_menu(struct game* game)
{
	UNUSED_PARAMETER(game);
}

void update_play(struct game* game)
{
	if (!game->paused)
	{
		// Update player.
		if ((g_player.actor.velocity.y + (float)GRAVITY) <= T_VEL) // Player can't exceed terminal velocity.
			g_player.actor.velocity.y += (float)GRAVITY;
		else
			player_set_vel_y(&g_player, T_VEL);
		// Move him.
		player_move(&g_player, (vec2) { (coord)g_player.actor.velocity.x, (coord)g_player.actor.velocity.y });
		update_player_double_jump(&g_player);
		update_player_draw_state(&g_player);
		// Update camera.
		camera_set(&g_camera, (vec2) { g_player.actor.skeleton.x - CENTER_X, g_player.actor.skeleton.y - CENTER_Y });
		if (g_player.actor.hitpoints == 0)
		{
			INFO("YOU DIED!");
			sound_play("death");
			game_set_state(game, MENU);
		}
	}
}

void update_edit(struct game* game)
{
	UNUSED_PARAMETER(game);
	camera_scroll(&g_camera, (vec2) { (coord) g_player.actor.velocity.x*g_player.actor.skeleton.w, (coord) g_player.actor.velocity.y*g_player.actor.skeleton.h });
}