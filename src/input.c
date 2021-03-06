#include <SDL.h>

#include "actor.h"
#include "game.h"
#include "camera.h"
#include "collision.h"
#include "common.h"
#include "level.h"
#include "menu.h"
#include "object.h"
#include "sound.h"
#include "input.h"
#include "replay.h"
#include "physics.h"
#include "timer.h"

int curr_sprite_num = 1;

static void toggle_fullscreen(SDL_Window* Window) {
	Uint32 FullscreenFlag = SDL_WINDOW_FULLSCREEN;
	bool IsFullscreen = SDL_GetWindowFlags(Window) & FullscreenFlag;
	SDL_SetWindowFullscreen(Window, IsFullscreen ? 0 : FullscreenFlag);
	SDL_ShowCursor(IsFullscreen);
}

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
			case SDLK_f:
				toggle_fullscreen(game->screen.window);
				break;
			case SDLK_RETURN:
				// TODO: Redesign - send accept message to menu.
				sound_play("accept");
				if (SDL_strcmp(M_MENU_PLAY, g_menu->button_list->current->text) == 0)
					game_state_change(game, game_state_preplay());
				else if (SDL_strcmp(M_MENU_EDIT, g_menu->button_list->current->text) == 0)
					game_state_change(game, game_state_preedit());
				else if (SDL_strcmp(M_MENU_REPLAY, g_menu->button_list->current->text) == 0)
				{
					char* name = malloc(SDL_strlen("level1") + 1);
					SDL_strlcpy(name, "level1", SDL_strlen("level1") + 1);
					game_state_change(game, game_state_replay(name));
				}
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
					g_player.action[ACTION_RIGHT] = true;
					break;
				case SDLK_LEFT:
				case SDLK_a:
					g_player.action[ACTION_LEFT] = true;
					break;
				case SDLK_DOWN:
				case SDLK_s:
					g_player.action[ACTION_DOWN] = true;
					break;
				case SDLK_UP:
				case SDLK_w:
					g_player.action[ACTION_UP] = true;
					break;
				case SDLK_t:
					timer_reset(&g_timer);
					break;
				case SDLK_f:
					toggle_fullscreen(game->screen.window);
					break;
				case SDLK_e:
					if(game_mode == MODE_PACIFIC)
						player_eat(&g_player);
					break;
				case SDLK_SPACE:
					g_player.action[ACTION_JUMP] = true;
					break;
				case SDLK_RETURN:
					game_pause(game);
					break;
				default:
					break;
				}
				break;
			case SDL_KEYUP:
				switch (event.key.keysym.sym) {
				case SDLK_RIGHT:
				case SDLK_d:
					g_player.action[ACTION_RIGHT] = false;
					break;
				case SDLK_LEFT:
				case SDLK_a:
					g_player.action[ACTION_LEFT] = false;
					break;
				case SDLK_ESCAPE:
					game_state_reset(game);
					break;
				case SDLK_DOWN:
				case SDLK_s:
					g_player.action[ACTION_DOWN] = false;
					break;
				case SDLK_UP:
				case SDLK_w:
					g_player.action[ACTION_UP] = false;
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
					if (m_pos.x > (g_player.actor.skeleton.x - g_camera.position.x))
						g_player.action[ACTION_SHOOT_RIGHT] = true;
					else
						g_player.action[ACTION_SHOOT_LEFT] = true;
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
	// Save the replay.
	/*
	ACTION_NONE,
	ACTION_LEFT,
	ACTION_RIGHT,
	ACTION_UP,
	ACTION_DOWN,
	ACTION_JUMP,
	ACTION_SHOOT_LEFT,
	ACTION_SHOOT_RIGHT,*/
	replay_save(g_player.action[ACTION_NONE]);
	replay_save(g_player.action[ACTION_LEFT]);
	replay_save(g_player.action[ACTION_RIGHT]);
	replay_save(g_player.action[ACTION_UP]);
	replay_save(g_player.action[ACTION_DOWN]);
	replay_save(g_player.action[ACTION_JUMP]);
	replay_save(g_player.action[ACTION_SHOOT_LEFT]);
	replay_save(g_player.action[ACTION_SHOOT_RIGHT]);
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
				if (SDL_strcmp(MENU_OK, g_menu->button_list->current->text) == 0)
				{
					// BEWARE: Possible error here. Can text get freed before the state exits?
					if (!game_state_change(game, game_state_edit(g_menu->text_box_list->current->text)))
					{
						INFO("Not loaded!");
						game_state_exit(game);
					}
				}
				else if (SDL_strcmp(MENU_CANCEL, g_menu->button_list->current->text) == 0)
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

bool process_input_preplay(struct game* game)
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
			if (e.key.keysym.sym == SDLK_UP || e.key.keysym.sym == SDLK_LEFT)
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
				if (SDL_strcmp(P_MENU_LEVEL1, g_menu->button_list->current->text) == 0)
				{
					// BEWARE: Possible error here. Can text get freed before the state exits?
					if (!game_state_change(game, game_state_mode(P_MENU_LEVEL1)))
					{
						INFO("Not loaded!");
						game_state_exit(game);
					}
				}
				else if (SDL_strcmp(P_MENU_LEVEL2, g_menu->button_list->current->text) == 0)
				{
					// BEWARE: Possible error here. Can text get freed before the state exits?
					if (!game_state_change(game, game_state_mode(P_MENU_LEVEL2)))
					{
						INFO("Not loaded!");
						game_state_exit(game);
					}
				}
				else if (SDL_strcmp(MENU_CANCEL, g_menu->button_list->current->text) == 0)
					game_state_exit(game);
			}
		}
	}
	return true;
}

bool process_input_replay(struct game* game)
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
		else if (e.type == SDL_KEYDOWN)
		{
			if (e.key.keysym.sym == SDLK_ESCAPE)
			{
				// exit to main menu
				game_state_reset(game);
				return true;
			}
		}
	}
	// Read input from file.
	replay_read_frame(g_player.action);
	return true;
}

bool process_input_mode(struct game* game)
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
			if (e.key.keysym.sym == SDLK_UP || e.key.keysym.sym == SDLK_LEFT)
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
				if (SDL_strcmp(P_DIFFICULTY_NORMAL, g_menu->button_list->current->text) == 0)
				{
					game_mode = MODE_NORMAL;
					if (!game_state_change(game, game_state_play(game->run->state_param)))
					{
						INFO("Not loaded!");
						game_state_exit(game);
					}
				}
				else if (SDL_strcmp(P_DIFFICULTY_EXTERMINATION, g_menu->button_list->current->text) == 0)
				{
					game_mode = MODE_EXTERMINATION;
					// BEWARE: Possible error here. Can text get freed before the state exits?
					if (!game_state_change(game, game_state_play(game->run->state_param)))
					{
						INFO("Not loaded!");
						game_state_exit(game);
					}
				}
				else if (SDL_strcmp(P_DIFFICULTY_PACIFIC, g_menu->button_list->current->text) == 0)
				{
					game_mode = MODE_PACIFIC;
					// BEWARE: Possible error here. Can text get freed before the state exits?
					if (!game_state_change(game, game_state_play(game->run->state_param)))
					{
						INFO("Not loaded!");
						game_state_exit(game);
					}
				}
				else if (SDL_strcmp(MENU_CANCEL, g_menu->button_list->current->text) == 0)
					game_state_exit(game);
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
				game_state_change(game, game_state_play(g_level->name));
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
			case SDLK_f:
				toggle_fullscreen(game->screen.window);
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
			case SDLK_c:
			{
				// Set the destination point
				vec2 position = { 0, 0 };
				SDL_GetMouseState(&position.x, &position.y);
				g_level->goal.x = position.x + g_camera.position.x - g_player.actor.skeleton.w / 2;
				g_level->goal.y = position.y + g_camera.position.y - g_player.actor.skeleton.h / 2;
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
			case SDLK_ESCAPE:
				game_state_reset(game);
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

static void update_player(struct player* player, struct game* game)
{
	player_set_vel_x(&g_player, 0);
	g_player.climb[0] = g_player.climb[1] = false;
	if (player->action[ACTION_LEFT])
		player_set_vel_x(&g_player, -g_player.actor.speed);
	if(player->action[ACTION_RIGHT])
		player_set_vel_x(&g_player, g_player.actor.speed);
	
	if(player->action[ACTION_DOWN])
		g_player.climb[0] = true;
	
	if(player->action[ACTION_UP])
		g_player.climb[1] = true;
	
	if (player->action[ACTION_JUMP])
	{
		player_jump(&g_player, PLAYER_JUMP_INTENSITY);
		g_player.action[ACTION_JUMP] = false;
	}
	float vel = 7.f;
	if (player->action[ACTION_SHOOT_RIGHT])
	{
		missile_fire(&g_player, &vel, game->screen.renderer);
		player->action[ACTION_SHOOT_RIGHT] = false;
	}
	else if (player->action[ACTION_SHOOT_LEFT])
	{
		vel = -vel;
		missile_fire(&g_player, &vel, game->screen.renderer);
		player->action[ACTION_SHOOT_LEFT] = false;
	}
	player_gravity(player);
	// Move him.
	vec2 delta = {
		(coord)g_player.actor.velocity.x,
		(coord)g_player.actor.velocity.y
	};
	player_move(player, &delta);
	if (player_can_climb(player))
		player_climb(player);
	if (g_player.actor.hitpoints <= 0)
	{
		INFO("YOU DIED!");
		sound_play("death");
		game_state_exit(game);
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
	player->actor.anim.curr->delay_counter += 1000/FPS;
	if (player->actor.anim.curr->delay_counter > player->actor.anim.curr->delay)
	{
		animation_next(&player->actor.anim);
		player->actor.anim.curr->delay_counter = 0;
	}
}

// check if the goal has been reached by the player, if so, next level
static void goal_update(struct game* game)
{
	if (g_level)
	{
		SDL_Rect goal_skeleton = { g_level->goal.x, g_level->goal.y, g_level->tile_map.tile_width, g_level->tile_map.tile_height };
		if (rects_collide(&g_player.actor.skeleton, &goal_skeleton))
		{
			if (game_mode == MODE_EXTERMINATION && SDL_strcmp("level3", g_level->name) != 0) // level3 is exempt from the EXTERMINATION requirement
			{
				struct enemy *iter = g_enemies;
				while (iter)
				{
					if (iter->is_spawned)
					{
						return;
					}
					iter = iter->next;
				}
			}
			sound_play("win");
			// there is a next lvel
			if (g_level->next[0] != '\0')
			{
				char* name = malloc(SDL_strlen(g_level->next) + 1);
				SDL_strlcpy(name, g_level->next, SDL_strlen(g_level->next) + 1);
				game_state_exit(game);
				if (!game_state_change(game, game_state_play(name)))
				{
					INFO("Not loaded!");
					game_state_exit(game);
				}
				free(name);
			}
			else
			{
				game_state_exit(game);
			}
		}
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
		timer_ticks(&g_timer);
		// Update camera.
		camera_set(&g_camera, (vec2) { g_player.actor.skeleton.x - CENTER_X, g_player.actor.skeleton.y - CENTER_Y });
		object_update_all(game->screen.renderer);
		enemy_update_all();
		update_player_draw_state(&g_player);
		update_player(&g_player, game);
		update_player_double_jump(&g_player);
		goal_update(game);
	}
}

void update_edit(struct game* game)
{
	UNUSED_PARAMETER(game);
	camera_scroll(&g_camera, (vec2) { (coord) g_player.actor.velocity.x*g_player.actor.skeleton.w, (coord) g_player.actor.velocity.y*g_player.actor.skeleton.h });
}