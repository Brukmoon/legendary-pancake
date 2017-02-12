#include <SDL.h>

#include "actor.h"
#include "game.h"
#include "camera.h"
#include "common.h"
#include "level.h"
#include "menu.h"
#include "object.h"
#include "sound.h"
#include "input.h"
#include "physics.h"

int curr_sprite_num = 1;


#include "path.h"

bool process_input_menu(struct game* game)
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
				if (SDL_strcmp(M_MENU_PLAY, g_menu->button_list->current->text) == 0)
					game_state_change(game, game_state_play("level1"));
				else if (SDL_strcmp(M_MENU_EDIT, g_menu->button_list->current->text) == 0)
					game_state_change(game, game_state_preedit());
				else if (SDL_strcmp(M_MENU_QUIT, g_menu->button_list->current->text) == 0)
				{
					game_state_exit(game);
					return false;
				}
				break;
			}
			break;
		case SDL_QUIT:
			game_state_reset(game);
			game_state_exit(game);
			INFO("EXIT flag set.");
			return false;
			break;
		}
	}
	return true;
}

bool process_input_play(struct game* game)
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
				case SDLK_d:
					player_set_vel_x(&g_player, g_player.actor.speed);
					break;
				case SDLK_LEFT:
				case SDLK_a:
					player_set_vel_x(&g_player, -g_player.actor.speed);
					break;
				case SDLK_s:
					g_player.climb[0] = true;
					break;
				case SDLK_w:
					g_player.climb[1] = true;
					break;
				/*case SDLK_e:
					game_state_change(game, game_state_edit(NULL));
					break;*/
				case SDLK_SPACE:
					player_jump(&g_player, PLAYER_JUMP_INTENSITY);
					break;
				case SDLK_RETURN:
					game_pause(game);
					break;
				case SDLK_j:
				{
					path_destroy(&g_player.path);
					path_find((vec2) { 25, 0 }, (vec2) { (int)g_player.actor.skeleton.x / g_level->tile_map.tile_width, (int)g_player.actor.skeleton.y / g_level->tile_map.tile_height }, &g_player.path);
					
				}
					break;
				default:
					break;
				}
				break;
			case SDL_KEYUP:
				switch (event.key.keysym.sym) {
				case SDLK_RIGHT:
				case SDLK_LEFT:
				case SDLK_d:
				case SDLK_a:
					player_set_vel_x(&g_player, 0);
					break;
				case SDLK_q:
					game_state_exit(game);
					break;
				case SDLK_s:
					g_player.climb[0]= false;
					break;
				case SDLK_w:
					g_player.climb[1] = false;
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
					// mouse pos
					vec2 m_pos = { 0,0 };
					SDL_GetMouseState(&m_pos.x, &m_pos.y);
					float vel = m_pos.x > (g_player.actor.skeleton.x-g_camera.position.x) ? 7.f : -7.f;
					missile_fire(&g_player, &vel, game->screen.renderer);
					break;
				}
				}
				break;
			case SDL_QUIT:
				game_state_reset(game);
				game_state_exit(game);
				INFO("EXIT flag set.");
				return false;
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
	return true;
}

bool process_input_preedit(struct game* game)
{
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT)
		{
			game_state_reset(game);
			game_state_exit(game);
			return false;
		}
		//Special key input
		else if (e.type == SDL_KEYDOWN)
		{
			//Handle backspace
			if (e.key.keysym.sym == SDLK_BACKSPACE)
			{
				if(SDL_strlen(g_menu->text_box_list->current->text) > 0)
					//lop off character
					g_menu->text_box_list->current->text[SDL_strlen(g_menu->text_box_list->current->text) - 1] = 0;
			}
			else if (e.key.keysym.sym == SDLK_UP || e.key.keysym.sym == SDLK_LEFT)
			{
				menu_prev_button(g_menu);
			}
			else if (e.key.keysym.sym == SDLK_DOWN || e.key.keysym.sym == SDLK_RIGHT)
			{
				menu_next_button(g_menu);
			}
			else if (e.key.keysym.sym == SDLK_RETURN)
			{
				INFO("Clicked button %s.", g_menu->button_list->current->text);
				sound_play("accept");
				if (SDL_strcmp(E_MENU_OK, g_menu->button_list->current->text) == 0)
				{
					// BEWARE: Possible error here. Can text get freed before the state exits?
					if (!game_state_change(game, game_state_edit(g_menu->text_box_list->current->text)))
					{
						INFO("Not loaded!");
						game_state_exit(game);
					}
				}
				else if (SDL_strcmp(E_MENU_CANCEL, g_menu->button_list->current->text) == 0)
					game_state_exit(game);
			}
			//Handle copy
			else if (e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
			{
				SDL_SetClipboardText(g_menu->text_box_list->current->text);
			}
			//Handle paste
			else if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
			{
				SDL_strlcpy(g_menu->text_box_list->current->text, SDL_GetClipboardText(), g_menu->text_box_list->current->max_length);
			}
		}
		//Special text input event
		else if (e.type == SDL_TEXTINPUT)
		{
			//Not copy or pasting
			if (!((e.text.text[0] == 'c' || e.text.text[0] == 'C') && (e.text.text[0] == 'v' || e.text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL))
			{	
				//Append character
				SDL_strlcat(g_menu->text_box_list->current->text, e.text.text, g_menu->text_box_list->current->max_length);
			}
		}
	}
	return true;
}

static void set_tile_texture(const vec2 *position, int value)
{
	INFO("Setting map tile texture at position [%d;%d] to value %d!", position->x, position->y, value);
	g_level->tile_map.map[TMAP_TEXTURE_LAYER][position->y][position->x] = value;
}
static void toggle_map_tile_coll(const vec2 *position)
{
	tile t = !g_level->tile_map.map[TMAP_COLLISION_LAYER][position->y][position->x];
	INFO("Writing map tile collision data at position [%d;%d] to %s!", position->x, position->y, t?"TRUE":"FALSE");
	g_level->tile_map.map[TMAP_COLLISION_LAYER][position->y][position->x] = t;
}

static void toggle_map_ladder(const vec2 *position)
{
	tile t = ((g_level->tile_map.map[TMAP_COLLISION_LAYER][position->y][position->x]==2)?0:2);
	INFO("Writing map ladder data at position [%d;%d] to %s!", position->x, position->y, t ? "TRUE" : "FALSE");
	g_level->tile_map.map[TMAP_COLLISION_LAYER][position->y][position->x] = t;
}

static const vec2 calc_mouse_pos_map()
{
	vec2 position = { 0, 0 };
	SDL_GetMouseState(&position.x, &position.y);
	position.x = (int)(g_camera.position.x + position.x) / g_level->tile_map.tile_width;
	position.y = (int)(g_camera.position.y + position.y) / g_level->tile_map.tile_height;
	// "Out of map" cases.
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

bool process_input_edit(struct game* game)
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
				game_state_change(game, game_state_play("level0"));
				break;
			case SDLK_s:
				level_save();
				break;
			case SDLK_l:
			{
				// Toggle ladder collision.
				vec2 position = calc_mouse_pos_map();
				toggle_map_ladder(&position);
			}
			break;
			case SDLK_x:
			{
				// Set the spawn point for the player.
				vec2 position = { 0, 0 };
				SDL_GetMouseState(&position.x, &position.y);
				position.x = position.x + g_camera.position.x - g_player.actor.skeleton.w/2;
				position.y = position.y + g_camera.position.y - g_player.actor.skeleton.h/2;
				player_set_spawn(&g_player, position);
			}
				break;
			case SDLK_2:
			case SDLK_KP_2:
				tile_map_resize(&g_level->tile_map, RESIZE_HEIGHT, 1);
				break;
			case SDLK_6:
			case SDLK_KP_6:
				tile_map_resize(&g_level->tile_map, RESIZE_WIDTH, 1);
				break;
			case SDLK_4:
			case SDLK_KP_4:
				tile_map_resize(&g_level->tile_map, RESIZE_WIDTH, -1);
				break;
			case SDLK_8:
			case SDLK_KP_8:
				tile_map_resize(&g_level->tile_map, RESIZE_HEIGHT, -1);
				break;
			case SDLK_q:
				game_state_exit(game);
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
				set_tile_texture(&position, curr_sprite_num);
				break;
			}
			case SDL_BUTTON_RIGHT:
			{
				vec2 position = calc_mouse_pos_map();
				set_tile_texture(&position, 0);
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
			game_state_reset(game);
			game_state_exit(game);
			INFO("EXIT flag set.");
			return false;
			break;
		}
	}
	return true;
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
	/*
	if (player->actor.state == GROUND)
	{
		if (player->actor.draw_state < player->actor.sprite_count - 1)
			player->actor.draw_state += .2f;
		else
			player->actor.draw_state = 0;
	}
	else if (player->actor.state == LADDER && player->actor.velocity.y)
	{
		if (player->actor.draw_state < player->actor.sprite_count - 1)
			player->actor.draw_state += .2f;
		else
			player->actor.draw_state = 0;
	}
	*/
	player->actor.anim.curr->delay_counter += 1000/FPS;
	if (player->actor.anim.curr->delay_counter > player->actor.anim.curr->delay)
	{
		animation_next(&player->actor.anim);
		player->actor.anim.curr->delay_counter = 0;
	}
	object_update();
}

void update_menu(struct game* game)
{
	UNUSED_PARAMETER(game);
}

void update_play(struct game* game)
{
	if (!game->paused)
	{
		path_destroy(&g_player.path);
		path_find((vec2) { 25, 0 }, (vec2) { (int)g_player.actor.skeleton.x / g_level->tile_map.tile_width, (int)g_player.actor.skeleton.y / g_level->tile_map.tile_height }, &g_player.path);
		// Update player.
		if ((g_player.actor.velocity.y + (float)GRAVITY) <= T_VEL) // Player can't exceed terminal velocity.
			g_player.actor.velocity.y += (float)GRAVITY;
		else
			player_set_vel_y(&g_player, T_VEL);
		// Move him.
		vec2 delta = {
			(coord)g_player.actor.velocity.x,
			(coord)g_player.actor.velocity.y
		};
		player_move(&g_player, &delta);
		if (player_can_climb(&g_player))
			player_climb(&g_player);
		update_player_double_jump(&g_player);
		update_player_draw_state(&g_player);
		// Update camera.
		camera_set(&g_camera, (vec2) { g_player.actor.skeleton.x - CENTER_X, g_player.actor.skeleton.y - CENTER_Y });
		if (g_player.actor.hitpoints == 0)
		{
			INFO("YOU DIED!");
			sound_play("death");
			game_state_exit(game);
		}
	}
}

void update_edit(struct game* game)
{
	UNUSED_PARAMETER(game);
	camera_scroll(&g_camera, (vec2) { (coord) g_player.actor.velocity.x*g_player.actor.skeleton.w, (coord) g_player.actor.velocity.y*g_player.actor.skeleton.h });
}