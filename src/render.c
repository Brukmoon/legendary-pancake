#include "actor.h"
#include "camera.h"
#include "common.h"
#include "config.h"
#include "collision.h"
#include "game.h"
#include "graphics.h"
#include "level.h"
#include "menu.h"
#include "object.h"
#include "render.h"

#if SHOW_TIME
#include "text.h"
#include "timer.h"
#endif // SHOW_SCORE

#if SHOW_CONSOLE
#include "text.h"
#endif // _DEBUG

#ifndef BLANK_TILE
#define BLANK_TILE 0
#endif // BLANK_TILE

#define TILE_HEIGHT g_level->tile_map.tile_height
#define TILE_WIDTH g_level->tile_map.tile_width
// editor collision rectangle size
#define COLL_RECT_SIZE TILE_WIDTH/4
#define MAP_NOT_OVERFLOW (y>=0&&y<=g_level->tile_map.height&&x>=0&&x<=g_level->tile_map.width)

static void draw_map_grid(SDL_Renderer *const renderer);
// play cursor
static void draw_cursor(const char* name, SDL_Renderer *renderer);
// Draws the main menu.
static void draw_menu_interface(SDL_Renderer* const renderer);
// draw dynamic map background (moving one)
static void draw_dyn_map_background(SDL_Renderer* const renderer);
// Draws the curr_map map to the screen.
static void draw_map(SDL_Renderer* const renderer, const enum render_map_flags f);
// draw actor paths
static void draw_path(SDL_Renderer* const renderer);
// draw current score
static void draw_score(SDL_Renderer* const renderer);

static void draw_map(SDL_Renderer* const renderer, const enum render_map_flags f)
{
	// draw backgrounds
	SDL_RenderCopy(renderer, g_level->background, NULL, NULL);
	draw_dyn_map_background(renderer);

	int start_y = g_camera.position.y/TILE_HEIGHT, start_x = g_camera.position.x/TILE_WIDTH;
	vec2 position = { 0,0 };
	for (int y = start_y; y < g_level->tile_map.height; ++y)
		for (int x = start_x; x < g_level->tile_map.width; ++x)
		{
			position.x = x*TILE_WIDTH;
			position.y = y*TILE_HEIGHT;
			if (!is_visible(&g_camera, &position, TILE_WIDTH, TILE_HEIGHT))
				break;
			if (MAP_NOT_OVERFLOW)
			{
				if (g_level->tile_map.map[TMAP_TEXTURE_LAYER][y][x] != BLANK_TILE)
				{
					// TODO: Move the SDL_Rect outside the function call
					SDL_RenderCopy(renderer, g_level->tileset, &(const struct SDL_Rect){ (((int)g_level->tile_map.map[TMAP_TEXTURE_LAYER][y][x] % 16)-1)*TILE_WIDTH, ((int)g_level->tile_map.map[TMAP_TEXTURE_LAYER][y][x]/16)*TILE_HEIGHT,TILE_WIDTH, TILE_HEIGHT },
						&(const struct SDL_Rect){ x*TILE_WIDTH - (g_camera.position.x), y*TILE_HEIGHT - (g_camera.position.y), TILE_WIDTH, TILE_HEIGHT });
				}
				// render collision rects
				if (f & RENDER_COLL)
				{
					if (g_level->tile_map.map[TMAP_COLLISION_LAYER][y][x] == TILE_COLLISION)
						fill_rect(renderer, (x + 1)*TILE_WIDTH - (g_camera.position.x) - COLL_RECT_SIZE, y*TILE_HEIGHT - (g_camera.position.y),
							COLL_RECT_SIZE, COLL_RECT_SIZE, (SDL_Color) {
						255, 0, 0, 1
					});
					else if (g_level->tile_map.map[TMAP_COLLISION_LAYER][y][x] == LADDER_COLLISION)
						fill_rect(renderer, (x + 1)*TILE_WIDTH - (g_camera.position.x) - COLL_RECT_SIZE, y*TILE_HEIGHT - (g_camera.position.y),
							COLL_RECT_SIZE, COLL_RECT_SIZE, (SDL_Color) {
						0, 255, 0, 1
					});
				}
			}
			else
			{
				if (g_camera.t == CAMERA_FIXED)
					break;
			}
		}
	// render map grid
	if (f & RENDER_GRID)
		draw_map_grid(renderer);
}

#undef MAP_NOT_OVERFLOW

#if SHOW_CONSOLE

static void render_debug_console(SDL_Renderer* const renderer)
{
	char buffer[50];
	// renderer
	SDL_RendererInfo rendererInfo;
	SDL_GetRendererInfo(renderer, &rendererInfo);
	sprintf_s(buffer, 50, "Graphics API: %s %d x %d", rendererInfo.name, SCREEN_WIDTH, SCREEN_HEIGHT);
	draw_text(buffer, 12, (SDL_Color) { 0, 0, 0 }, (vec2) { 0, 0 }, renderer);
	// coordinates
	sprintf_s(buffer, 50, "Player coordinates: [%d;%d]", g_player.actor.skeleton.x, g_player.actor.skeleton.y);
	draw_text(buffer, 12, (SDL_Color) { 0, 0, 0 }, (vec2) { 0, 12 }, renderer);
	// speed
	sprintf_s(buffer, 50, "Player speed: [%f;%f]", g_player.actor.velocity.x, g_player.actor.velocity.y);
	draw_text(buffer, 12, (SDL_Color) { 0, 0, 0 }, (vec2) { 0, 24 }, renderer);
	// player animation
	sprintf_s(buffer, 50, "Player animation state: %s", g_player.actor.anim.curr->name);
	draw_text(buffer, 12, (SDL_Color) { 0, 0, 0 }, (vec2) { 0, 36 }, renderer);
	// state
	switch (g_player.actor.state)
	{
	case AIR:
		sprintf_s(buffer, 50, "AIR");
		break;
	case GROUND:
		sprintf_s(buffer, 50, "GROUND");
		break;
	case LADDER:
		sprintf_s(buffer, 50, "LADDER");
		break;
	default:
		sprintf_s(buffer, 50, "Unknown");
		break;
	}
	draw_text(buffer, 12, (SDL_Color) { 0, 0, 0 }, (vec2) { 0, 48 }, renderer);

}
#endif // _DEBUG

void draw_menu_interface(SDL_Renderer* const renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
	menu_draw(g_menu, renderer);
}

static void draw_player_info(const struct player *player, SDL_Renderer *renderer)
{
	// draw hp
	fill_rect(renderer, SCREEN_WIDTH - 105, 9, player->actor.hitpoints, 11, (SDL_Color) { 255, 0, 0, 1 });
	hollow_rect(renderer, SCREEN_WIDTH - 105, 9, 100, 11, (SDL_Color) { 0, 0, 0, 1 });
	// stamina
	if (player->actor.jump_count < 2)
		fill_rect(renderer, SCREEN_WIDTH - 110, 5, 5, 10, (SDL_Color) { 255, 255, 0, 1 });
	if (player->actor.jump_count < 1)
		fill_rect(renderer, SCREEN_WIDTH - 110, 15, 5, 10, (SDL_Color) { 255, 255, 0, 1 });
	hollow_rect(renderer, SCREEN_WIDTH - 110, 5, 5, 10, (SDL_Color) { 0, 0, 0, 1 });
	hollow_rect(renderer, SCREEN_WIDTH - 110, 15, 5, 10, (SDL_Color) { 0, 0, 0, 1 });

	SDL_Rect *src_rect = NULL;
	SDL_Rect dest_rect = (SDL_Rect) { SCREEN_WIDTH-135, 5, 20, 20 };
	// avatar
	SDL_Texture *t = sprite_get(player->actor.anim.curr->curr->sprite_name, &src_rect);
	SDL_RenderCopy(renderer, t, src_rect, &dest_rect);
	if(player->collect > 0)
	{
		t = sprite_get("bamboo", &src_rect);
		dest_rect = (SDL_Rect) { SCREEN_WIDTH - 30, 35, 32, 32 };
		SDL_RenderCopy(renderer, t, src_rect, &dest_rect);
		// write bamboo count
		char buffer[5];
		SDL_itoa(player->collect, buffer, 10);
		draw_text(buffer, 20, (SDL_Color) { 255, 255, 255, 1 }, (vec2) { SCREEN_WIDTH - 35, 33 }, renderer);
	}
}

void render_play(SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 1);
	SDL_RenderClear(renderer);

#if SHOW_GRID
	draw_map(renderer, RENDER_GRID);
#else
	draw_map(renderer, RENDER_BASE);
#endif // SHOW_GRID
#if SHOW_CONSOLE
	render_debug_console(renderer);
#endif // SHOW_CONSOLE
	// draw all objects, e.g. missiles, static objects etc.
	object_draw(renderer);
	player_draw(&g_player, renderer);
	enemy_draw_all(renderer);
	draw_player_info(&g_player, renderer);
	// draw cursor if you can shoot
	if(player_can_shoot(&g_player))
		draw_cursor("arrow", renderer);
	draw_path(renderer);
	draw_score(renderer);

	SDL_RenderPresent(renderer);
}

void render_menu(SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, 112, 146, 190, 1);
	SDL_RenderClear(renderer);

	draw_menu_interface(renderer);

	SDL_RenderPresent(renderer);
}

void render_edit(SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 1);
	SDL_RenderClear(renderer);

	draw_map(renderer, RENDER_ALL);
	int x = 0, y = 0;
	SDL_GetMouseState(&x, &y);
	// cursor
	hollow_rect(renderer, x-(g_player.actor.skeleton.w)/2, y- (g_player.actor.skeleton.h) / 2, g_player.actor.skeleton.w, g_player.actor.skeleton.h,
		(SDL_Color) {
		255, 0, 0, 0
	});
	// current sprite
	SDL_RenderCopy(renderer, g_level->tileset, &(const struct SDL_Rect){ ((curr_sprite_num%16)-1)* TILE_WIDTH, (curr_sprite_num / 16)*TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT },
		&(const struct SDL_Rect){ x - (g_player.actor.skeleton.w) / 2, y - (g_player.actor.skeleton.h) / 2, TILE_WIDTH, TILE_HEIGHT });
	// spawn
	hollow_rect(renderer, g_player.actor.spawn.x - g_camera.position.x,g_player.actor.spawn.y - g_camera.position.y, g_player.actor.skeleton.w, g_player.actor.skeleton.h,
		(SDL_Color) {
		0, 0, 255, 0
	});
	// goal
	hollow_rect(renderer, g_level->goal.x - g_camera.position.x, g_level->goal.y - g_camera.position.y, g_player.actor.skeleton.w, g_player.actor.skeleton.h,
		(SDL_Color) {
		255, 255, 0, 0
	});

	SDL_RenderPresent(renderer);
}

static void draw_map_grid(SDL_Renderer *const renderer)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 1);
	for (int i = 0; i < g_level->tile_map.width+1; ++i)
		SDL_RenderDrawLine(renderer, i*TILE_WIDTH - (g_camera.position.x), 0 - (g_camera.position.y), i*TILE_WIDTH - (g_camera.position.x), TILE_HEIGHT*g_level->tile_map.height - (g_camera.position.y));
	for (int i = 0; i < g_level->tile_map.height+1; ++i)
		SDL_RenderDrawLine(renderer, 0 - (g_camera.position.x), i*TILE_HEIGHT - (g_camera.position.y), TILE_WIDTH*g_level->tile_map.width - (g_camera.position.x), i*TILE_HEIGHT - (g_camera.position.y));
}

static void draw_cursor(const char* name, SDL_Renderer *renderer)
{
	SDL_Rect *src = NULL;
	SDL_Rect dst = { 0,0, TILE_WIDTH,TILE_HEIGHT };
	// where is the mouse?
	SDL_GetMouseState(&dst.x, &dst.y);
	SDL_Texture *texture = sprite_get(name, &src);
	// is the player to the right of the mouse?
	if(dst.x > (g_player.actor.skeleton.x-g_camera.position.x))
		SDL_RenderCopy(renderer, texture, src, &dst);
	else // else flip
		SDL_RenderCopyEx(renderer, texture, src, &dst, 0,0, SDL_FLIP_HORIZONTAL);

}

static void draw_dyn_map_background(SDL_Renderer* const renderer)
{
	if (g_level->d_background)
	{
		// TODO: move static variable to level struct so it won't be alive the entire time
		static float scrollingOffset = 0;
		scrollingOffset -= 0.5f;
		int w = 0, h = 0;
		SDL_QueryTexture(g_level->d_background, NULL, NULL, &w, &h);
		if ((int)scrollingOffset < -w)
		{
			scrollingOffset = 0;
		}
		SDL_Rect dst_rect = { (int)scrollingOffset, 0, w, h };
		SDL_RenderCopy(renderer, g_level->d_background, NULL, &dst_rect);
		dst_rect.x = (int)scrollingOffset + w;
		SDL_RenderCopy(renderer, g_level->d_background, NULL, &dst_rect);
	}
}

static void draw_path(SDL_Renderer* const renderer)
{
#ifdef _DEBUG
	struct enemy* enemy_current = g_enemies;
	while (enemy_current)
	{
		struct waypoint* current = enemy_current->path;
		// is there a path?
		if (enemy_current->current)
		{
			while (current)
			{
				SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);
				vec2 waypoint_pos = { current->pos.x * 32, current->pos.y * 32 };
				if (is_visible(&g_camera, &waypoint_pos, TILE_WIDTH, TILE_HEIGHT))
					hollow_rect(renderer, waypoint_pos.x - g_camera.position.x, waypoint_pos.y - g_camera.position.y, TILE_WIDTH, TILE_HEIGHT, (SDL_Color) { 255, 255, 255, 0 });
				current = current->next;
			}
			hollow_rect(renderer, enemy_current->current->pos.x * TILE_WIDTH - g_camera.position.x, enemy_current->current->pos.y * TILE_HEIGHT - g_camera.position.y, TILE_WIDTH, TILE_HEIGHT, (SDL_Color) { 255, 0, 0, 0 });
		}
		enemy_current = enemy_current->next;
	}
#endif // _DEBUG
}

static void draw_score(SDL_Renderer* const renderer)
{
#if SHOW_TIME
	int delta_seconds = g_timer.delta / 1000;
	if (delta_seconds != g_timer.draw_time_seconds)
	{
		g_timer.draw_time_seconds = delta_seconds;
		char time[12], minutes[5], seconds[3];
		int time_minutes = g_timer.draw_time_seconds / 60, time_seconds = g_timer.draw_time_seconds % 60;
		// dont go over 99
		if (time_minutes > 99)
			time_minutes = 99;
		SDL_itoa(time_minutes, minutes, 10);
		SDL_itoa(time_seconds, seconds, 10);
		time[0] = '\0';
		// add a trailing zero
		if(time_minutes < 10)
			SDL_strlcat(time, "0", 10);
		SDL_strlcat(time, minutes, 10);
		SDL_strlcat(time, ":", 10);
		if (time_seconds < 10)
			SDL_strlcat(time, "0", 10);
		SDL_strlcat(time, seconds, 10);
		g_timer.time = create_text_texture(renderer, time, 32, (SDL_Color) { 255, 255, 255, 1 });
	}
	SDL_Rect dst_rect = (SDL_Rect){ CENTER_X, 0, 65, 45 };
	SDL_RenderCopy(renderer, g_timer.time, NULL, &dst_rect);
#endif // SHOW_TIME
}